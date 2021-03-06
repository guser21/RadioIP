//
// Created by guser on 5/23/18.
//

#include <server/control_daemon.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <common/err.h>
#include <unistd.h>
#include <cstring>
#include <sstream>
#include <utility>
#include <thread>
#include <server/server_parser.h>
#include <iostream>

void ControlDaemon::start() {
    auto th = std::thread([&] {
        this->request_handler();
    });
    th.detach();
}

void ControlDaemon::setup() {
    sockaddr_in hostaddr{};
    bzero(&hostaddr, sizeof(hostaddr));

    ctrl_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (ctrl_socket < 0) Err::syserr("ctrl socket");

    hostaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    hostaddr.sin_port = htons(ctrl_port);
    hostaddr.sin_family = AF_INET;
    int optval = 1;
    if (setsockopt(ctrl_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        Err::syserr("setsockopt on control daemon server");

    optval = TTL_VALUE;
    if (setsockopt(ctrl_socket, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &optval, sizeof optval) < 0)
        Err::syserr("setsockopt multicast ttl");


    int rtvl = bind(ctrl_socket, (struct sockaddr *) &hostaddr, sizeof(hostaddr));
    if (rtvl < 0) Err::syserr("error in bind ctrl_socket");
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
    res += '\n';
    return res;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
void ControlDaemon::request_handler() {
    char buffer[CTRL_BUFFER_SIZE];
    ssize_t read_bytes, snd_len;
    struct sockaddr_in client_addr{};
    socklen_t socklen = sizeof(client_addr);
    ServerParser parser;
    bzero(buffer, sizeof(buffer));
    bzero(&client_addr, sizeof(client_addr));


    while (true) {
        read_bytes = recvfrom(ctrl_socket, buffer, sizeof(buffer), 0, (struct sockaddr *) &client_addr, &socklen);
        if (read_bytes <= 0) Err::logerr("recvfrom in control port");

        if (strncmp(buffer, LOOKUP_MSG, read_bytes) == 0) {
            std::string reply = this->station_addr();
            snd_len = sendto(ctrl_socket, reply.c_str(), reply.size(), 0,
                             (struct sockaddr *) &client_addr, socklen);
            if (snd_len != reply.size()) Err::logerr("sendto in control daemon request handler ");
        }

        if (strncmp(buffer, RETRY_MSG, strlen(RETRY_MSG)) == 0) {
            auto res = parser.parse_requests(std::string(buffer));
            {
                std::unique_lock<std::mutex> lock(retr_req->mut);
                for (auto item: res) {
                    retr_req->elems.insert(item);
                }
            }
            std::cerr << "received request " << buffer << std::endl;
        }

        bzero(&client_addr, sizeof(client_addr));
        bzero(buffer, sizeof(buffer));
    }
}
#pragma clang diagnostic pop


ControlDaemon::ControlDaemon(ServerOptions serverOptions, SetMutex<uint64_t> *vecMutex) : retr_req(vecMutex) {
    this->ctrl_port = serverOptions.ctrl_port;
    this->mcast_addr = serverOptions.mcast_addr;
    this->data_port = serverOptions.data_port;
    this->station_name = serverOptions.station_name;

}

