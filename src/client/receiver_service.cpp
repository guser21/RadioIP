//
// Created by guser on 5/24/18.
//
#include <algorithm>
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
#include <client/receiver_service.h>
#include <iostream>
#include <common/packet_dto.h>


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void ReceiverService::restart(Strategy strategy, Station station) {
    auto previous_station = session.station;
    session.clean();
    buffer->clean();

    connections[2].events = 0;
    session.current_status = Status::DOWN;
    if (connections[1].fd != -1) disconnect();

    switch (strategy) {
        case Strategy::CONNECT_FIRST:
            if (!stations.empty()) {
                connections[1].fd = connect(stations[0]);
                connections[1].events = POLLIN;

                session.station = stations[0];
                session.current_status = Status::WAITING_FIRST_PACKET;

                retransmissionService.restart(session.station.address);
            } else {
                session.current_status = Status::DOWN;
            }
            break;
        case Strategy::RECONNECT:
            session.station = previous_station;
            session.current_status = Status::WAITING_FIRST_PACKET;

            connections[1].fd = connect(previous_station);
            connections[1].events = POLLIN;

            retransmissionService.restart(session.station.address);
            break;
        case Strategy::CONNECT_THIS:
            session.station = station;
            session.current_status = Status::WAITING_FIRST_PACKET;

            connections[1].fd = connect(station);
            connections[1].events = POLLIN;

            retransmissionService.restart(station.address);
    }
    update_ui();
}

void ReceiverService::update_ui() {
    uiService.update_view(stations, session.station);
    for (int i = 4; i < connections.size(); ++i) {
        connections[i].events = POLLIN | POLLOUT;
    }
}

void ReceiverService::check_timeout() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto cur_time = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    for (int i = 0; i < stations.size(); ++i) {
        if ((cur_time - stations[i].last_discover) > DROP_TIMEOUT) {
            auto erased_station = stations[i];
            stations.erase(stations.begin() + i);
            if (session.station == erased_station) {
                restart(Strategy::CONNECT_FIRST, InvalidStation);
                std::cerr << "dropping" << std::endl;
            }
        }
    }
}


void ReceiverService::lookup_handler(int fd, uint16_t event) {
    bzero(read_buffer, MAX_UDP_SIZE);
    sockaddr_in server_address{};
    socklen_t len = sizeof(server_address);
    auto read_bytes = recvfrom(fd, read_buffer, MAX_UDP_SIZE, 0,
                               (struct sockaddr *) &server_address, &len);
    if (read_bytes <= 0) {
        Err::logerr("read in 0th fd poll");
        restart(Strategy::CONNECT_FIRST, InvalidStation);
    }

    discover_handler(read_buffer, server_address);

    if (session.current_status == Status::DOWN && !stations.empty()) {
        if (prefer_station) {
            for (auto &station: stations) {
                station.name == this->prefered_station;
                restart(Strategy::CONNECT_THIS, station);
                break;
            }
        } else {
            restart(Strategy::CONNECT_FIRST, InvalidStation);
        }
    }
}

void ReceiverService::current_server_io_handler(int fd, uint16_t event) {
    bzero(read_buffer, MAX_UDP_SIZE);
    Packet *packet;

    auto read_bytes = read(fd, read_buffer, MAX_UDP_SIZE);
    int psize = read_bytes - sizeof(Packet) + 1;

    packet = reinterpret_cast<Packet *> (read_buffer);
    packet->session_id = be64toh(packet->session_id);
    packet->first_byte_num = be64toh(packet->first_byte_num);


    if (session.current_status == Status::WAITING_FIRST_PACKET) {
        session.current_status = Status::ACTIVE;

        session.session_id = packet->session_id;
        session.byte0 = packet->first_byte_num;
        session.max_packet_id = packet->first_byte_num;
        session.psize = psize;
    }

    if (session.current_status == Status::ACTIVE) {
        if (packet->first_byte_num > session.max_packet_id + psize) {

            int max_pack = (buffer_size / psize);
            ssize_t from = std::max(
                    static_cast<long long>(packet->first_byte_num) - (max_pack * psize),
                    static_cast<long long>(session.max_packet_id) + psize);

            retransmissionService.add_request(from, packet->first_byte_num, psize);
        }
        if (packet->first_byte_num < session.max_packet_id) {
            std::cerr << "received retransmissed data " << packet->first_byte_num << std::endl;
            retransmissionService.notify(packet->first_byte_num);
        }
    }


    if (packet->first_byte_num >= session.byte0 && packet->session_id == session.session_id) {
        session.max_packet_id = std::max(packet->first_byte_num, session.max_packet_id);
        buffer->push(packet->audio_data, psize, packet->first_byte_num);

        if (session.max_packet_id > session.byte0 + 3 * (buffer_size / 4)) {
            connections[2].events = POLLOUT;
        }
    }


    if (packet->session_id > session.session_id) {
        restart(Strategy::RECONNECT, InvalidStation);
    }
}

void ReceiverService::start() {
    int nfds;

    //it is guaranteed that vector allocates continuous memory blocks
    //TODO change -1 to  500
    while ((nfds = poll(&connections[0], connections.size(), 500)) >= 0) {
        if (nfds < 0) Err::syserr("error in poll");
        check_timeout();
        if (connections[0].revents & POLLIN) {
            lookup_handler(connections[0].fd, connections[0].revents);
        }
        //current server
        if (connections[1].revents & POLLIN) {
            current_server_io_handler(connections[1].fd, connections[1].revents);
        }

        //STDOUT
        if (connections[2].revents & POLLOUT) {

            auto readable = buffer->read();

            if (readable.first == 0) {
                restart(Strategy::RECONNECT, InvalidStation);
            } else {
                auto written_data = write(STDOUT_FILENO, readable.second, readable.first);
                fflush(stdout);
                buffer->commit_read(written_data);
            };
        }
        //UI central accept connections
        if (connections[3].revents & POLLIN) {
            int fd = uiService.accept_connection();

            struct pollfd ui_client;
            ui_client.revents = 0;
            ui_client.events = POLLOUT | POLLIN;
            ui_client.fd = fd;

            connections.push_back(ui_client);

            update_ui();
        }


        std::vector<int> drop_ui;
        for (int i = 4; i < connections.size(); i++) {
            auto resp = uiService.handle_io(connections[i].fd, connections[i].revents);
            int next_station, prev_station, max_id = static_cast<int>(stations.size());
            int cur_stationID = -1;

            for (int j = 0; j < stations.size(); ++j) {
                if (stations[j] == session.station) {
                    cur_stationID = j;
                }
            }

            switch (resp) {
                case Response::REMOVE:
                    drop_ui.push_back(i);
                case Response::UP:
                    if (cur_stationID != -1) {
                        prev_station = ((cur_stationID - 1 + max_id) % max_id);
                        restart(Strategy::CONNECT_THIS, stations[prev_station]);
                    }
                case Response::DOWN:
                    if (cur_stationID != -1) {
                        next_station = ((cur_stationID + 1 + max_id) % max_id);
                        restart(Strategy::CONNECT_THIS, stations[next_station]);
                    }
                case Response::NOMOREOUT:
                    connections[i].events = POLLIN;
                case Response::NONE:
                    break;
            }
        }

        for (int j = 0; j < drop_ui.size(); ++j) {
            connections.erase(connections.begin() + drop_ui[j]);
        }

        std::transform(connections.begin(), connections.end(), connections.begin(), [](struct pollfd p) {
            p.revents = 0;
            return p;
        });
    }
    Err::logerr("error in poll");
    delete read_buffer;
}

#pragma clang diagnostic pop


int ReceiverService::connect(Station cur_station) {
    struct sockaddr_in local_address{};

    int stream_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (stream_sock < 0)Err::syserr("socket");

    request.imr_interface.s_addr = htonl(INADDR_ANY);

    if (inet_aton(cur_station.mcast_addr.c_str(), &request.imr_multiaddr) == 0)
        Err::syserr("inet_aton");

    if (setsockopt(stream_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &request, sizeof request) < 0)
        Err::syserr("setsockopt");

    int option = 1;
    if (setsockopt(stream_sock, SOL_SOCKET, SO_REUSEADDR, (void *) &option, sizeof option) < 0)
        Err::syserr("setsockopt");

    bzero(&local_address, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(cur_station.data_port);

    if (bind(stream_sock, (struct sockaddr *) &local_address, sizeof local_address) < 0)
        Err::syserr("bind");

    data_socket = stream_sock;
    return stream_sock;
}

void ReceiverService::disconnect() {
    if (setsockopt(data_socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (void *) &request, sizeof request) < 0)
        Err::syserr("setsockopt");
    close(data_socket);
}


void ReceiverService::discover_handler(char *msg, sockaddr_in server_address) {

    auto new_station = client_parser.parse(msg);
    if (new_station == InvalidStation) return;

    server_address.sin_port = htons(ctrl_port);
    new_station.address = server_address;

    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto cur_time = std::chrono::duration_cast<std::chrono::seconds>(now).count();
    bool found = false;//TODO not sure

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
        update_ui();
    }
}

ReceiverService::ReceiverService(DiscoverService &discoverService,
                                 UIService &uiService,
                                 RetransmissionService &retransmissionService,
                                 ClientOptions clientOptions) :
        discoverService(discoverService),
        retransmissionService(retransmissionService),
        uiService(uiService) {
    this->buffer_size = clientOptions.buffer_size;
    this->rtime = clientOptions.rtime;
    this->ctrl_port = clientOptions.ctrl_port;
    this->prefered_station = clientOptions.prefered_station;
    this->prefer_station = clientOptions.prefer_station;
    this->read_buffer = new char[MAX_UDP_SIZE];


    this->discoverService.setup();
    this->discoverService.start();
    this->server_reply_sock = this->discoverService.get_disc_sock();

    this->buffer = new Buffer(buffer_size);

    this->uiService.setup();

    this->setup();
}

/*
 *Convention
 *  0 - server_reply socket UDP
 *  1 - current server socket UDP
 *  2 - std out non blocking//TODO not sure
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
    if (fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK) < 0) Err::syserr("could not set stdout to nonblock");

    struct pollfd ui_connection;
    ui_connection.fd = uiService.get_reg_socket();
    ui_connection.events = POLLIN;
    ui_connection.revents = 0;
    connections.push_back(ui_connection);
}

ReceiverService::~ReceiverService() {
    delete buffer;
    delete read_buffer;
}
