//
// Created by guser on 5/24/18.
//

#include <client/reciever_service.h>
#include <cstdio>
#include <zconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <utility>
#include <common/err.h>
#include <common/const.h>
#include <common/packet_dto.h>
#include <strings.h>
#include <thread>
#include <algorithm>
//TODO dropout


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

//TODO refactor too big function
//TODO TCP takes too many cycles of cpu
void ReceiverService::start() {
    int nfds;
    char reply_buffer[CTRL_BUFFER_SIZE];
    char read_buffer[10 * sizeof(Packet)], packet_buffer[sizeof(Packet)];
    ssize_t read_bytes;
    int packed_bytes = 0;
    Packet *packet;


    //it is guaranteed that vector allocates continuous memory blocks
    while ((nfds = poll(&connections[0], connections.size(), -1) >= 0)) {
        if (connections[0].revents & POLLIN) {
            nfds--;
            bzero(reply_buffer, CTRL_BUFFER_SIZE);

            //TODO do in while until everything is read
            read_bytes = read(connections[0].fd, reply_buffer, sizeof(reply_buffer));
            discover_handler(reply_buffer);

            //TODO refactor really bad code
            if (connections[1].fd == -1 && !stations.empty()) {
                //TODO option with -n
                connections[1].fd = get_station_fd(stations[0]);
            }
        }

        if (connections[1].revents & POLLIN) {
            nfds--;
            read_bytes = read(connections[1].fd, read_buffer, sizeof(read_buffer));
            if (read_bytes < sizeof(Packet)) continue;

            for (int j = 0; j < read_bytes; ++j) {
                packet_buffer[packed_bytes] = read_buffer[j];
                packed_bytes++;
                if (packed_bytes == sizeof(Packet)) {
                    packet = reinterpret_cast<Packet *> (packet_buffer);
                    printf("session_id %lu \n", packet->session_id);
                    printf("first_byte %lu\n", packet->first_byte_num);
                    write(STDOUT_FILENO, packet->audio_data, PSIZE);
                    printf("\n");

                    packed_bytes = 0;
                }
            }
        }


        //TODO ui ports

        std::transform(connections.begin(), connections.end(), connections.begin(), [](struct pollfd p) {
            p.revents = 0;
            return p;
        });
    }
    if (nfds < 0) perror("error in poll");
}

#pragma clang diagnostic pop


int ReceiverService::get_station_fd(Station cur_station) {


    struct sockaddr_in local_address{};
    struct ip_mreq ip_mreq{};

    int stream_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (stream_sock < 0)syserr("socket");

    ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    if (inet_aton(cur_station.mcast_addr.c_str(), &ip_mreq.imr_multiaddr) == 0)
        syserr("inet_aton");

    if (setsockopt(stream_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &ip_mreq, sizeof ip_mreq) < 0)
        syserr("setsockopt");

    bzero(&local_address, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(cur_station.port);

    //TODO doesn't work ask
//    connect(stream_sock, (struct sockaddr *) &local_address, sizeof(local_address));
    if (bind(stream_sock, (struct sockaddr *) &local_address, sizeof local_address) < 0)
        syserr("bind");
    return stream_sock;
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

ReceiverService::ReceiverService(DiscoverService &discoverService, UIService &uiService) :
        discoverService(discoverService),
        uiService(uiService) {
    this->discoverService.setup();
    this->discoverService.start();

    this->server_reply_sock = discoverService.get_disc_sock();

    this->setup();

    //TODO BAD DESIGN
    this->uiService.setStations(&stations);
    this->uiService.setup();

    this->ui_socket = uiService.get_reg_socket();

}

/*
 *Convention
 *  0 - server_reply socket UDP
 *  1 - current server socket UDP
 *  2 - ui register socket TCP
 * */
void ReceiverService::setup() {


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

    struct pollfd ui_server;
    ui_server.fd = ui_socket;
    ui_server.events = POLLIN;
    ui_server.revents = 0;
    connections.push_back(ui_server);

}
