//
// Created by guser on 5/26/18.
//

#ifndef RADIO_STATION_H
#define RADIO_STATION_H

#include <string>
#include <utility>

class Station {
public:
    Station() {}

    Station(const std::string &name, const std::string &mcast_addr, uint16_t port, time_t last_discover)
            : name(name), mcast_addr(mcast_addr), port(port), last_discover(last_discover) {}

    std::string name;
    std::string mcast_addr;
    uint16_t port;
    time_t last_discover;

    friend bool operator==(const Station &L, const Station &R) {
        return (L.mcast_addr == R.mcast_addr)
               && (L.port == R.port)
               && (L.name == R.name);
    }

};

extern Station InvalidStation;


#endif //RADIO_STATION_H
