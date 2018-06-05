//
// Created by guser on 5/26/18.
//

#ifndef RADIO_STATION_H
#define RADIO_STATION_H

#include <string>
#include <utility>
#include <netinet/in.h>

class Station {
public:
    Station() = default;

    std::string name;
    std::string mcast_addr;
    uint16_t data_port;
    time_t last_discover;
    sockaddr_in address;

    Station(const std::string &name, const std::string &mcast_addr, uint16_t data_port, time_t last_discover) :
            name(name), mcast_addr(mcast_addr), data_port(data_port),
            last_discover(last_discover) {}

    friend bool operator==(const Station &L, const Station &R) {
        return (L.mcast_addr == R.mcast_addr)
               && (L.data_port == R.data_port)
               && (L.name == R.name);
    }

};

extern Station InvalidStation;


#endif //RADIO_STATION_H
