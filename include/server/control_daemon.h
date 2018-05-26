//
// Created by guser on 5/23/18.
//

#ifndef RADIO_CONTROLDAEMON_H
#define RADIO_CONTROLDAEMON_H

#include <string>
#include <mutex>
#include "common/const.h"

class ControlDaemon {
private:

    int ctrl_port;
    std::string mcast_addr;
    int data_port;
    int ctrl_socket;
    std::string station_name;

    void request_handler();

public:
    ControlDaemon(int ctrl_port, std::string mcast_addr, int data_port, std::string station_name);
    void setup();

    void start();

    std::string station_addr();
};


#endif //RADIO_CONTROLDAEMON_H
