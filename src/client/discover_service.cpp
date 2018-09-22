//
// Created by guser on 5/26/18.
//

#include <string>
#include <client/discover_service.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <unistd.h>
#include <common/const.h>
#include <iostream>
#include <client/station.h>
#include <sstream>
#include <vector>
#include <mutex>
#include <common/err.h>

void DiscoverService::setup() {
    disc_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (disc_socket < 0) Err::syserr("discover service socket");

    struct sockaddr_in disc_addr{};
    bzero(&disc_addr, sizeof(disc_addr));

    disc_addr.sin_family = AF_INET;
    disc_addr.sin_port = htons(ctrl_port);

    int rtvl = inet_pton(AF_INET, discover_addr.c_str(), &disc_addr.sin_addr);//ip address
    if (rtvl < 0) Err::syserr("inet_pton discover service");

    int enable_broadcast = 1;
    rtvl = setsockopt(disc_socket, SOL_SOCKET, SO_BROADCAST, &enable_broadcast, sizeof(enable_broadcast));
    if (rtvl < 0) Err::syserr("setsockopt failed in discover connect");

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
            if (written_data != sizeof(LOOKUP_MSG)) Err::logerr("write to disc socket");
            sleep(DISCOVER_REPEAT);
        }
    }).detach();
}


#pragma clang diagnostic pop

DiscoverService::DiscoverService(const std::string &discover_addr, uint16_t ctrl_port,
                                 unsigned int rtime) : discover_addr(discover_addr),
                                                       ctrl_port(ctrl_port), rtime(rtime) {}
