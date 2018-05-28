//
// Created by guser on 5/23/18.
//

#ifndef RADIO_CONTROLDAEMON_H
#define RADIO_CONTROLDAEMON_H

#include <string>
#include <mutex>
#include "common/const.h"
#include "server_options.h"

class ControlDaemon {
private:

    uint16_t ctrl_port;
    std::string mcast_addr;
    uint16_t data_port;
    uint16_t ctrl_socket;
    std::string station_name;

    void request_handler();

public:
    explicit ControlDaemon(ServerOptions serverOptions);

    void setup();

    void start();

    std::string station_addr();
};


#endif //RADIO_CONTROLDAEMON_H
