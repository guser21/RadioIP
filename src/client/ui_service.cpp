//
// Created by guser on 5/28/18.
//

#include <string>
#include <client/ui_service.h>
#include <netinet/in.h>
#include <common/err.h>
#include <common/const.h>
#include <iostream>
#include <zconf.h>
#include <fcntl.h>

void UIService::setup() {
    sockaddr_in server_address{};

    reg_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (reg_socket < 0) syserr("socket");

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(ui_port);

    int option = 1;
    if (setsockopt(reg_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
        syserr("socket option in ui client");
    // bind the socket to a concrete address
    if (bind(reg_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        syserr("bind in ui client");

    if (listen(reg_socket, QUEUE_LENGTH) < 0) syserr("listen");
}

int UIService::get_reg_socket() {
    return reg_socket;
}


UIService::UIService(uint16_t ui_port) :ui_port(ui_port) {}

void UIService::update_view(std::vector<Station> &stations, Station &current_station) {
    view.clear();

    view += LINE;
    view += "\n\r";

    view += CLIENT_NAME;
    view += "\n\r";

    view += LINE;
    view += "\n\r";
    for (auto &station: stations) {
        if (station == current_station) {
            view += "  > ";
        } else {
            view += "    ";
        }
        view += station.name;
        view += "\n\r";
    }
}

int UIService::accept_connection() {
    UIClient client;
    socklen_t client_address_len = sizeof(client.client_address);
    int fd = accept(reg_socket, (struct sockaddr *) &client.client_address, &client_address_len);
    if (fd < 0) logerr("accept connection in ui");

    //7 bytes is not enough to get blocked by write
    if (write(fd, LINEMODE, 7) != 7) logerr("telnet not configured");

    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) logerr("could not set client fd to nonblock");

    clients[fd]=client;
}

