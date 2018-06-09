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
#include <strings.h>
#include <poll.h>

static bool ends_with(std::string &fullString, std::string ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare(fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void UIService::setup() {
    sockaddr_in server_address{};

    reg_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (reg_socket < 0) Err::syserr("socket");

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(ui_port);

    int option = 1;
    if (setsockopt(reg_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
        Err::syserr("socket option in ui client");
    // bind the socket to a concrete address
    if (bind(reg_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        Err::syserr("bind in ui client");

    if (listen(reg_socket, QUEUE_LENGTH) < 0) Err::syserr("listen");
}

int UIService::get_reg_socket() {
    return reg_socket;
}

Response UIService::handle_io(int fd, short event) {
    auto &client = clients[fd];
    if (event & POLLERR) {
        close(fd);
        std::cerr << "ui service fd err" << std::endl;
        return Response::REMOVE;
    }

    if (event & POLLIN) {
        bzero(read_buffer, TCP_READ_BUFFER);
        auto read_bytes = read(fd, read_buffer, TCP_READ_BUFFER);
        if (read_bytes <= 0) {
            close(fd);
            std::cerr << "ui connection lost" << std::endl;
            return Response::REMOVE;
        }
        client.input_buffer.append(read_buffer, read_bytes);
        if (ends_with(client.input_buffer, std::string(UP_KEY, sizeof(UP_KEY)))) {
            client.input_buffer.clear();
            return Response::UP;
        }

        if (ends_with(client.input_buffer, std::string(DOWN_KEY, sizeof(DOWN_KEY)))) {
            client.input_buffer.clear();
            return Response::DOWN;
        }
    }

    if (event & POLLOUT) {
        if (client.clear_pos < sizeof(CLEAR)) {
            auto cl_pos = write(fd, CLEAR + client.clear_pos, sizeof(CLEAR) - client.clear_pos);
            if (cl_pos < 0) {
                close(fd);
                std::cerr << "ui service write" << std::endl;
                return Response::REMOVE;
            }
            client.clear_pos += cl_pos;
        }

        if (client.clear_pos == sizeof(CLEAR)) {
            auto written = write(fd, view.c_str() + client.view_pos, view.size() - client.view_pos);
            if (written < 0) {
                close(fd);
                std::cerr << "ui service write" << std::endl;
                return Response::REMOVE;
            }
            client.view_pos += written;
        }

        if (client.view_pos == view.size()) {
            return Response::NOMOREOUT;
        }
    }
    return Response::NONE;
}


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
    view += LINE;

    for (auto &cl:clients) {
        cl.second.view_pos = 0;
        cl.second.clear_pos = 0;
    }
}

int UIService::accept_connection() {
    UIClient client;
    socklen_t client_address_len = sizeof(client.client_address);
    int fd = accept(reg_socket, (struct sockaddr *) &client.client_address, &client_address_len);
    if (fd < 0) Err::logerr("accept connection in ui");

    //7 bytes is not enough to get blocked by write
    if (write(fd, LINEMODE, 7) != 7) Err::logerr("telnet not configured");

    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) Err::logerr("could not set client fd to nonblock");

    client.fd = fd;
    clients[fd] = client;

    return fd;
}

UIService::UIService(uint16_t ui_port) : ui_port(ui_port) {
    read_buffer = new char[TCP_READ_BUFFER];
}

UIService::~UIService() {
    delete read_buffer;
}

