//
// Created by guser on 6/3/18.
//

#include <vector>
#include <server/server_parser.h>
#include <sstream>

static std::vector<std::string> split(const std::string &s, char delim) {
    std::stringstream ss(s);
    std::string item;
    std::vector<std::string> tokens;
    while (getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

static bool are_digits(const std::string &num) {
    bool isnum = true;
    for (auto ch : num) {
        isnum &= isdigit(ch);
    }
    return isnum;
}

//TODO TEST
std::vector<uint64_t> ServerParser::parse_requests(std::string str) {
    std::vector<uint64_t> vec;
    auto tokens = split(str, ',');
    for (auto &req:tokens) {
        if (are_digits(req)) {
            vec.push_back(std::stoll(req));
        }
    }

    return vec;
}
