//
// Created by guser on 5/28/18.
//

#ifndef RADIO_OPTIONS_H
#define RADIO_OPTIONS_H

#include <string>

struct ClientOptions {
    std::string discover_addr;
    uint16_t ctrl_port;
    uint16_t ui_port;
    unsigned int buffer_size;
    unsigned int rtime;
    std::string prefered_station;
    bool prefer_station= false;
};

class ClientOptionsParser {
public:
    ClientOptions parse(int argc, char *argv[]);
};

#endif //RADIO_OPTIONS_H
