//
// Created by guser on 5/28/18.
//

#ifndef RADIO_UI_SERVICE_H
#define RADIO_UI_SERVICE_H

#include <vector>
#include "station.h"
//TODO const correctness

class UIService {
private:
    int reg_socket;
    uint16_t ui_port;
    const std::vector<Station>* stations;

    void setReg_socket(int reg_socket);
public:

    void setup();

    void setStations(const std::vector<Station> *stations);

    int get_reg_socket() const;

    std::string get_current_view();

    explicit UIService(uint16_t ui_port);
};

#endif //RADIO_UI_SERVICE_H
