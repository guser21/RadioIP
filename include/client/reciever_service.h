//
// Created by guser on 5/24/18.
//

#ifndef RADIO_RECIEVER_SERVICE_H
#define RADIO_RECIEVER_SERVICE_H

#include <string>

class ReceiverService {
private:
    int stream_sock;
    std::string mcast_address;
    int port;
public:
    ReceiverService(std::string mcast_address, int port);

    void setup();

    void start();
};

#endif //RADIO_RECIEVER_SERVICE_H
