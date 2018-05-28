//
// Created by guser on 5/24/18.
//

#ifndef RADIO_RECIEVER_SERVICE_H
#define RADIO_RECIEVER_SERVICE_H

#include <string>
#include <common/vec_mutex.h>
#include "station.h"
#include "msg_parser.h"
#include "ui_service.h"
#include "discover_service.h"
#include <poll.h>


class ReceiverService {
private:
    std::vector<struct pollfd> connections;
    int server_reply_sock;
    MsgParser msgParser;
    std::vector<Station> stations;
    UIService &uiService;
    DiscoverService& discoverService;
    int ui_socket;

public:

    void start();


    explicit ReceiverService(DiscoverService& discoverService,UIService& uiService );

    int get_station_fd(Station cur_station);

    void discover_handler(const std::string &msg);

    void setup();

};

#endif //RADIO_RECIEVER_SERVICE_H
