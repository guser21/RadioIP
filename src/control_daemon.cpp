//
// Created by guser on 5/23/18.
//

#include <server/control_daemon.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <common/err.h>
#include <zconf.h>
#include <cstring>
#include <sstream>
#include <utility>
#include <thread>

void ControlDaemon::start() {
    auto th = std::thread([&]{
        this->request_handler();
    });
    th.detach();
}

void ControlDaemon::setup() {
    sockaddr_in hostaddr{};
    bzero(&hostaddr, sizeof(hostaddr));

    ctrl_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctrl_socket < 0) syserr("ctrl socket");

    hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    hostaddr.sin_port = htons(ctrl_port);
    hostaddr.sin_family = AF_INET;

    int rtvl = bind(ctrl_socket, (struct sockaddr *) &hostaddr, sizeof(hostaddr));
    if (rtvl < 0) syserr("error in bind ctrl_socket");
}


std::string ControlDaemon::station_addr() {
    std::string res;
    res += REPLY_MSG;
    res += " ";
    res += mcast_addr;
    res += " ";
    res += std::to_string(data_port);
    res += " ";
    res += station_name;
    return res;
}

void ControlDaemon::request_handler() {
    char buffer[CTRL_BUFFER_SIZE];
    ssize_t read_bytes, snd_len;
    sockaddr_in client_addr{};
    socklen_t socklen = sizeof(sockaddr_in);

    bzero(buffer, sizeof(buffer));
    bzero(&client_addr, sizeof(client_addr));


    while ((read_bytes = recvfrom(ctrl_socket, buffer, sizeof(buffer), 0,
                                  (struct sockaddr *) &client_addr, &socklen)) > 0) {
        if (strncmp(buffer, LOOKUP_MSG, read_bytes) == 0) {
            std::string reply = this->station_addr();
            snd_len = sendto(ctrl_socket, reply.c_str(), reply.size(), 0,
                             (struct sockaddr *) &client_addr, sizeof(client_addr));

            if (snd_len < 0) syserr("sendto in control daemon request handler ");
        } else {
            printf("TODO");
        }
    }
}

ControlDaemon::ControlDaemon(int ctrl_port, std::string mcast_addr, int data_port,
                             std::string station_name) : ctrl_port(ctrl_port), mcast_addr(std::move(mcast_addr)),
                                                         data_port(data_port),
                                                         station_name(std::move(station_name)) {}

