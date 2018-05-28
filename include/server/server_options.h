//
// Created by guser on 5/28/18.
//

#ifndef RADIO_OPTIONS_H
#define RADIO_OPTIONS_H

#include <string>

struct ServerOptions {
    std::string mcast_addr;
    std::string station_name;
    uint16_t data_port;
    uint16_t ctrl_port;
    int packet_size;
    int fifo_size;
    int rtime;
};

class ServerOptionParser {
public:
    ServerOptions parse(int argc, char *argv[]);
};

#endif //RADIO_OPTIONS_H
