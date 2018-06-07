//
// Created by guser on 5/28/18.
//

#ifndef RADIO_UI_SERVICE_H
#define RADIO_UI_SERVICE_H

#include <vector>
#include <map>
#include "station.h"

struct UIClient {
    int fd = -1;
    int view_pos = 0;
    int clear_pos = 0;
    struct sockaddr_in client_address{};
    //collect chars until make sense then clean the buffer
    std::string input_buffer;
};


enum class Response {
    UP, DOWN, NONE, REMOVE, NOMOREOUT
};

class UIService {
private:
    std::string view;
    int reg_socket;
    uint16_t ui_port;
    std::map<int, UIClient> clients;
    char *read_buffer;
public:

    int accept_connection();

    void update_view(std::vector<Station> &stations, Station &current_station);

    void setup();

    int get_reg_socket();

    ~UIService();

    explicit UIService(uint16_t ui_port);

    Response handle_io(int fd, short event);
};

#endif //RADIO_UI_SERVICE_H
