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

enum class Status {
    ACTIVE,
    DOWN,
    WAITING_FIRST_PACKET
};
enum class Strategy {
    CONNECT_FIRST,
    RECONNECT,
    CONNECT_THIS
};

struct Session {
    Status current_status = Status::DOWN;
    uint64_t byte0 = 0;
    uint64_t session_id = 0;
    Station station = InvalidStation;
    uint64_t max_packet_id = 0;
    int psize = 0;

    void clean() {
        psize = 0;
        max_packet_id = 0;
        byte0 = 0;
        session_id = 0;
        current_status = Status::DOWN;
        station = InvalidStation;
    }
};


class ReceiverService {
private:
    std::vector<struct pollfd> connections;
    int server_reply_sock;
    std::vector<Station> stations;
    ClientParser client_parser;

    UIService &uiService;
    DiscoverService &discoverService;
    RetransmissionService &retransmissionService;
    Session session;
    Buffer *buffer;


    int rtime;
    int buffer_size;
    int data_socket;
    uint16_t ctrl_port;

    std::string prefered_station;
    bool prefer_station = false;

    struct ip_mreq request{};
    char *read_buffer;
public:
    virtual ~ReceiverService();

    void start();

    ReceiverService(DiscoverService &discoverService, UIService &uiService,
                    RetransmissionService &retransmissionService, ClientOptions clientOptions);

    int connect(Station cur_station);

    void discover_handler(char *msg, sockaddr_in in);

    void setup();

    void disconnect();

    void restart(Strategy strategy, Station station);

    void check_timeout();

    void update_ui();

    void lookup_handler(int fd, uint16_t event);

    void current_server_io_handler(int fd, uint16_t event);
};

#endif //RADIO_RECIEVER_SERVICE_H
