//
// Created by guser on 5/24/18.
//

#ifndef RADIO_RECIEVER_SERVICE_H
#define RADIO_RECIEVER_SERVICE_H

#include <string>
#include <common/vec_mutex.h>
#include "station.h"
#include "msg_parser.h"
#include <poll.h>


class ReceiverService {
private:
    int stream_sock;
    std::string mcast_address;
    uint16_t port;
    std::vector<struct pollfd> connections;
    int server_reply_sock;
    MsgParser msgParser;
    std::vector<Station> stations;
public:

    void start();


    explicit ReceiverService(int server_reply_sock);

    int get_station_fd(Station cur_station);

    void discover_handler(const std::string &msg);

    void setup();


    int get_uisocket();
};

#endif //RADIO_RECIEVER_SERVICE_H
