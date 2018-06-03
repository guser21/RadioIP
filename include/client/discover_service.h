//
// Created by guser on 5/26/18.
//

#ifndef RADIO_LOOKUP_SERVICE_H
#define RADIO_LOOKUP_SERVICE_H

#include <string>
#include <common/vec_mutex.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "station.h"

class DiscoverService {

private:
    std::string discover_addr;
    int disc_socket;
    uint16_t ctrl_port;
    unsigned int rtime;
    sockaddr_in broadcast_addr{};
public:
    void setup();
    DiscoverService(const std::string &discover_addr, uint16_t ctrl_port, unsigned int rtime);


    int get_disc_sock() const;

    void start();
};

#endif //RADIO_LOOKUP_SERVICE_H
