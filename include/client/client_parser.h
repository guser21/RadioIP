//
// Created by guser on 5/28/18.
//

#ifndef RADIO_MSG_PARSER_H
#define RADIO_MSG_PARSER_H

#include "station.h"

class ClientParser{
public:
    Station parse(std::string msg);
};
#endif //RADIO_MSG_PARSER_H
