//
// Created by guser on 5/28/18.
//

#include <string>
#include <client/ui_service.h>
#include <netinet/in.h>
#include <common/err.h>
#include <common/const.h>

std::string UIService::get_current_view() {
    return nullptr;
}

void UIService::setup() {
    sockaddr_in server_address{};

    reg_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (reg_socket < 0) syserr("socket");

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(ui_port);

    //TODO EVERYWHERE
    int option = 1;
    if (setsockopt(reg_socket, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)) < 0)
        syserr("socket option");
    // bind the socket to a concrete address
    if (bind(reg_socket, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
        syserr("bind");

    if (listen(reg_socket, QUEUE_LENGTH) < 0) syserr("listen");
}

int UIService::get_reg_socket() const {
    return reg_socket;
}

void UIService::setReg_socket(int reg_socket) {
    UIService::reg_socket = reg_socket;
}

UIService::UIService(uint16_t ui_port) {

}

void UIService::setStations(const std::vector<Station> *stations) {
    UIService::stations = stations;
}
