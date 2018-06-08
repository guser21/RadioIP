//
// Created by guser on 5/28/18.
//


#include <cctype>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstdio>
#include <sstream>
#include <vector>
#include <client/station.h>
#include <client/client_parser.h>
#include <common/const.h>

static std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

static bool isValidIpAddress(const std::string &ipAddress) {
    struct sockaddr_in sa{};
    int result = inet_pton(AF_INET, ipAddress.c_str(), &(sa.sin_addr));
    return result != 0;
}

static bool isNumber(const std::string &num) {
    bool isnum = true;
    for (auto ch : num) {
        isnum &= isdigit(ch);
    }
    return isnum;
}

Station ClientParser::parse(std::string msg) {
    Station station;
    char *end;

    auto tokens = split(msg, ' ');

    if (tokens.size() < 3) return InvalidStation;

    if (tokens[0] != REPLY_MSG) return InvalidStation;

    if (!isValidIpAddress(tokens[1])) return InvalidStation;

    auto port = atoi(tokens[2].c_str());
    if (!isNumber(tokens[2]) || port > MAX_PORT || port < 1) return InvalidStation;

    unsigned long name_ind = tokens[0].size() + tokens[1].size() + tokens[2].size() + 2;

    station.mcast_addr = tokens[1];
    station.data_port = static_cast<uint16_t>(port);
    station.name = msg.substr(name_ind + 1, msg.size() - name_ind - 2);

    station.last_discover = time(nullptr);//TODO change

    return station;
}
