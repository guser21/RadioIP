//
// Created by guser on 5/26/18.
//

#include <string>
#include <client/discover_service.h>
#include <sys/socket.h>
#include <common/err.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <zconf.h>
#include <common/const.h>
#include <iostream>
#include <client/station.h>
#include <sstream>
#include <vector>
#include <mutex>

void DiscoverService::setup() {
    disc_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (disc_socket < 0) syserr("discover service socket");

    struct sockaddr_in disc_addr{};
    bzero(&disc_addr, sizeof(disc_addr));

    disc_addr.sin_family = AF_INET;
    disc_addr.sin_port = htons(ctrl_port);//data_port

    int rtvl = inet_pton(AF_INET, discover_addr.c_str(), &disc_addr.sin_addr);//ip address
    if (rtvl < 0) syserr("inet_pton discover service");

    int enable_broadcast = 1;
    rtvl = setsockopt(disc_socket, SOL_SOCKET, SO_BROADCAST, &enable_broadcast, sizeof(enable_broadcast));
    if (rtvl < 0) syserr("setsockopt failed in discover connect");

    struct sockaddr_in local_address{};
    bzero(&local_address, sizeof(local_address));

//OK
    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(0);

    rtvl = bind(disc_socket, (struct sockaddr *) &local_address, sizeof(local_address));
    if (rtvl < 0) syserr("bind in discover service connect");

    this->broadcast_addr = disc_addr;
}

int DiscoverService::get_disc_sock() const {
    return disc_socket;
}


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void DiscoverService::start() {
    std::thread([&] {
        ssize_t written_data;
        while (true) {
            written_data = sendto(disc_socket, LOOKUP_MSG,
                                  sizeof(LOOKUP_MSG), 0, (struct sockaddr *) &this->broadcast_addr,
                                  sizeof(this->broadcast_addr));
            if (written_data != sizeof(LOOKUP_MSG)) logerr("write to disc socket");
            sleep(1);//TODO every 5 seconds?
        }
    }).detach();
}


#pragma clang diagnostic pop

DiscoverService::DiscoverService(const std::string &discover_addr, uint16_t ctrl_port,
                                 unsigned int rtime) : discover_addr(discover_addr),
                                                       ctrl_port(ctrl_port), rtime(rtime) {}
