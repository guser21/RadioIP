//
// Created by guser on 5/24/18.
//

#ifndef RADIO_RECIEVER_SERVICE_H
#define RADIO_RECIEVER_SERVICE_H

#include <string>
#include <common/safe_structures.h>
#include "station.h"
#include "client_parser.h"
#include "ui_service.h"
#include "discover_service.h"
#include "client_options.h"
#include "buffer.h"
#include "retransmission_service.h"
#include <poll.h>

enum Status {
    ACTIVE,
    DOWN,
    WAITING_FIRST_PACKET
};
enum Strategy {
    CONNECT_FIRST,
    CLEAN_BUFFERS
};

struct Session {
    Status current_status = DOWN;
    uint64_t byte0 = 0;
    uint64_t session_id = 0;
    Station station = InvalidStation;
    uint64_t max_packet_id = 0;

    void clean() {
        max_packet_id = 0;
        byte0 = 0;
        session_id = 0;
        current_status = DOWN;
        station = InvalidStation;
    }
};


class ReceiverService {
private:
    std::vector<struct pollfd> connections;
    int server_reply_sock;
    ClientParser client_parser;
    std::vector<Station> stations;

    UIService &uiService;
    DiscoverService &discoverService;
    RetransmissionService &retransmissionService;
    Session session;
    int ui_socket;
    int rtime;
    int buffer_size;
    int data_socket;
    uint16_t ctrl_port;

    struct ip_mreq request{};

public:

    void start();

    ReceiverService(DiscoverService &discoverService, UIService &uiService,RetransmissionService& retransmissionService, ClientOptions clientOptions);

    int connect(Station cur_station);

    void discover_handler(char *msg, sockaddr_in in);

    void setup();

    void disconnect();

    void restart(Strategy r, Buffer &buffer);

    void check_timeout(Buffer &buffer);
};

#endif //RADIO_RECIEVER_SERVICE_H
