//
// Created by guser on 5/23/18.
//

#ifndef RADIO_CONTROLDAEMON_H
#define RADIO_CONTROLDAEMON_H

#include <string>
#include <mutex>
#include <common/vec_mutex.h>
#include "common/const.h"
#include "server_options.h"

class ControlDaemon {
private:

    uint16_t ctrl_port;
    std::string mcast_addr;
    uint16_t data_port;
    int ctrl_socket;
    std::string station_name;

    void request_handler();

    SetMutex<uint64_t > *retr_req;

public:
    ControlDaemon(ServerOptions serverOptions, SetMutex<uint64_t > *retr_req);

    void setup();

    void start();

    std::string station_addr();
};


#endif //RADIO_CONTROLDAEMON_H
