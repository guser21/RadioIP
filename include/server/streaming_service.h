//
// Created by guser on 5/23/18.
//

#ifndef RADIO_STREAMINGSERVICE_H
#define RADIO_STREAMINGSERVICE_H

#include <boost/circular_buffer.hpp>
#include <common/packet_dto.h>

class StreamingService {
private:
    int stream_sock;
    boost::circular_buffer<Packet> buffer;
    int input_fd;
    std::string mcast_address;
    uint16_t port;
    int packet_id = 0;
    uint64_t session_id;

public:
    virtual ~StreamingService();

    StreamingService(int input_fd, const std::string &mcast_address, int port);


    void setup();

    void start();
};


#endif //RADIO_STREAMINGSERVICE_H
