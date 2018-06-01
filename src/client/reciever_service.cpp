//
// Created by guser on 5/24/18.
//

#include <cstdio>
#include <zconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <utility>
#include <common/err.h>
#include <common/const.h>
#include <strings.h>
#include <thread>
#include <algorithm>
#include <fcntl.h>
#include <client/buffer.h>
#include <client/reciever_service.h>
#include <iostream>


struct __attribute__((packed)) Packet {
    uint64_t session_id;
    uint64_t first_byte_num;
    char *audio_data;
};


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

//TODO BAD CODE
void ReceiverService::restart(Reason r, Buffer &buffer) {
    session.clean();
    buffer.clean();

    connections[2].events = 0;
    connections[1].fd = -1;
    switch (r) {
        case DROP_STATION:
            if (!stations.empty()) {
                connections[1].fd = connect(stations[0]);
                session.station = stations[0];
            }
        case MISSING_PACKAGE:
            return;
        default:
            return;
    }

}

void ReceiverService::check_timeout(Buffer &buffer) {
    auto cur_time = time(nullptr);
    for (int i = 0; i < stations.size(); ++i) {
        if ((cur_time - stations[i].last_discover) > DROP_TIMEOUT) {
            stations.erase(stations.begin() + i);
            auto erased_station = stations[i];
            if (session.station == erased_station) {
                restart(DROP_STATION, buffer);
                std::cout << "dropping";
                std::flush(std::cout);
            }
        }
    }
}

//TODO refactor too big function
//TODO TCP takes too many cycles of cpu
void ReceiverService::start() {

    int nfds;
    char reply_buffer[READ_BUFFER_SIZE];
    auto read_buffer = new char[MAX_UDP_SIZE];
    Packet *packet;
    Buffer buffer(buffer_size);//TODO smth with this

    //it is guaranteed that vector allocates continuous memory blocks
    while ((nfds = poll(&connections[0], connections.size(), -1)) >= 0) {
        if (nfds < 0) perror("error in poll");
        check_timeout(buffer);
        //0 - server_reply socket UDP
        if (connections[0].revents & POLLIN) {
            nfds--;
            bzero(reply_buffer, CTRL_BUFFER_SIZE);

            auto read_bytes = read(connections[0].fd, reply_buffer, sizeof(reply_buffer));
            if (read_bytes < 0) logerr("read in 0th fd");
            discover_handler(reply_buffer);
            //TODO refactor really bad code
            if (connections[1].fd == -1 && !stations.empty()) {
                //TODO option with -n
                connections[1].fd = connect(stations[0]);
                connections[1].events = POLLIN;

                session.station = stations[0];
            }
        }

        //current server
        if (connections[1].revents & POLLIN) {
            nfds--;
            auto read_bytes = read(connections[1].fd, read_buffer, MAX_UDP_SIZE);
            // TODO improve, bad code
            packet = reinterpret_cast<Packet *> (read_buffer);
            // 2 8byte numbers
            buffer.push(read_buffer + 16, read_bytes - 16, packet->first_byte_num);

            if (session.byte0 == -1) {
                session.byte0 = packet->first_byte_num;
            }

            if (buffer.get_lastId() > session.byte0 + 3 * (buffer_size / 4)) {
                connections[2].events = POLLOUT;
            }
        }
        //STDOUT
        if (connections[2].revents & POLLOUT) {
            nfds--;
            auto readable = buffer.read();
            if (readable.first == 0) {
                restart(MISSING_PACKAGE, buffer);
                continue;
            }
            auto written_data = write(STDOUT_FILENO, readable.second, readable.first);
            fflush(stdout);
            buffer.commit_read(written_data);
        }

        //TODO ui ports

        std::transform(connections.begin(), connections.end(), connections.begin(), [](struct pollfd p) {
            p.revents = 0;
            return p;
        });
    }
    delete read_buffer;
}

#pragma clang diagnostic pop


int ReceiverService::connect(Station cur_station) {
    struct sockaddr_in local_address{};

    int stream_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (stream_sock < 0)syserr("socket");

    request.imr_interface.s_addr = htonl(INADDR_ANY);

    if (inet_aton(cur_station.mcast_addr.c_str(), &request.imr_multiaddr) == 0)
        syserr("inet_aton");

    if (setsockopt(stream_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &request, sizeof request) < 0)
        syserr("setsockopt");

    int option = 1;
    if (setsockopt(stream_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &option, sizeof option) < 0)
        syserr("setsockopt");

    bzero(&local_address, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(cur_station.port);

    //TODO doesn't work ask
//    connect(stream_sock, (struct sockaddr *) &local_address, sizeof(local_address));
    if (bind(stream_sock, (struct sockaddr *) &local_address, sizeof local_address) < 0)
        syserr("bind");

    data_socket = stream_sock;
    return stream_sock;
}

void ReceiverService::disconnect() {
    if (setsockopt(data_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void *) &request, sizeof request) < 0)
        syserr("setsockopt");
    close(data_socket);
}


void ReceiverService::discover_handler(const std::string &msg) {
    auto new_station = msgParser.parse(msg);
    if (new_station == InvalidStation) return;

    time_t cur_time = time(nullptr);
    bool found = false;

    for (auto &station : stations) {
        if (station == new_station) {
            station.last_discover = cur_time;
            found = true;
        }
    }

    if (!found) {
        stations.push_back(new_station);
        std::sort(stations.begin(), stations.end(), [](const Station &l, const Station &r) {
            return l.name.compare(r.name);
        });
    }

}

ReceiverService::ReceiverService(DiscoverService &discoverService, UIService &uiService, ClientOptions clientOptions) :
        discoverService(discoverService),
        uiService(uiService) {
    this->buffer_size = clientOptions.buffer_size;
    this->rtime = clientOptions.rtime;

    this->discoverService.setup();
    this->discoverService.start();

    this->server_reply_sock = discoverService.get_disc_sock();

    this->setup();

//    //TODO BAD DESIGN
//    this->uiService.setStations(&stations);
//    this->uiService.setup();

    this->ui_socket = uiService.get_reg_socket();

}

/*
 *Convention
 *  0 - server_reply socket UDP
 *  1 - current server socket UDP
 *  2 - std out non blocking
 *  3 - ui register socket TCP
 * */
void ReceiverService::setup() {


    session.clean();

    struct pollfd server_reply;
    server_reply.fd = this->server_reply_sock;
    server_reply.events = POLLIN;
    server_reply.revents = 0;
    connections.push_back(server_reply);

    struct pollfd current_server;
    current_server.fd = -1;
    current_server.events = POLLIN;
    current_server.revents = 0;
    connections.push_back(current_server);

    struct pollfd stdout_fd;
    stdout_fd.fd = STDOUT_FILENO;
    stdout_fd.events = 0;
    stdout_fd.revents = 0;
    connections.push_back(stdout_fd);

    //Setting stdout to non-block
    if (fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK) < 0) syserr("fcntl in setup");


    //TODO turn on ui service
    struct pollfd ui_server;
//    ui_server.fd = ui_socket;
    ui_server.fd = -1;
    ui_server.events = POLLIN;
    ui_server.revents = 0;
    connections.push_back(ui_server);
}
