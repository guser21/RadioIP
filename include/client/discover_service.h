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
public:
    int get_disc_sock() const;

private:
    uint16_t ctrl_port;
    unsigned int rtime;
    VecMutex<Station> *vecMutex;
    sockaddr_in broadcast_addr{};
public:

    DiscoverService(std::string discover_addr, uint16_t ctrl_port, unsigned int rtime, VecMutex<Station> *vecMutex);

    void setup();

    void start();
};

#endif //RADIO_LOOKUP_SERVICE_H
