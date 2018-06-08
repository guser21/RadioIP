//
// Created by guser on 6/3/18.
//

#include <vector>
#include <server/server_parser.h>
#include <sstream>
#include <common/const.h>

//TODO take to common file
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
    bool are_dig = true;
    for (auto ch : num) {
        are_dig &= isdigit(ch);
    }
    return are_dig;
}


std::vector<uint64_t> ServerParser::parse_requests(std::string str) {
    std::string prefix(RETRY_MSG);
    prefix += " ";
    if (str.find(prefix) != 0)
        return std::vector<uint64_t>();

    str = str.substr(prefix.length(),str.size()-prefix.size()-1);

    std::vector<uint64_t> result;
    auto tokens = split(str, ',');
    for (auto &req:tokens) {
        if (are_digits(req)) {
            result.push_back(std::stoll(req));
        } else {
            return std::vector<uint64_t>();
        }
    }

    return result;
}
