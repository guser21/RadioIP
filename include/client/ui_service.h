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
    int clear_pos = -1;
    //collect chars until make sense then clean the buffer
    std::string input_buffer;
    struct sockaddr_in client_address;
};


//enum class
class UIService {
private:
    std::string view;
    int reg_socket;
    uint16_t ui_port;
    std::map<int,UIClient> clients;
public:

    int accept_connection();

    void update_view(std::vector<Station> &stations, Station &current_station);

    void setup();

    int get_reg_socket();



    explicit UIService(uint16_t ui_port);
};

#endif //RADIO_UI_SERVICE_H
