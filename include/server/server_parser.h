//
// Created by guser on 6/3/18.
//

#ifndef RADIO_SERVER_PARSER_H
#define RADIO_SERVER_PARSER_H

#include <vector>
#include <string>
#include <cstdint>

class ServerParser{
public:
    std::vector<uint64_t> parse_requests(std::string str);
};
#endif //RADIO_SERVER_PARSER_H
