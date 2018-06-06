//
// Created by guser on 5/28/18.
//

#ifndef RADIO_UI_SERVICE_H
#define RADIO_UI_SERVICE_H

#include <vector>
#include "station.h"

class UIClient {
    int fd = -1;
    int sent_bytes = 0;
    //collect chars until make sense then clean the buffer
    std::string input_buffer;

};

class UIService {
private:
    std::string view;
    int reg_socket;
    uint16_t ui_port;
    const std::vector<Station> *stations;

    void setReg_socket(int reg_socket);

public:

    void accept_connection();

    void update_view(std::vector<Station> &stations, Station &current_station);

    void setup();

    int get_reg_socket();

    std::string get_current_view();

    explicit UIService(uint16_t ui_port);
};

#endif //RADIO_UI_SERVICE_H
