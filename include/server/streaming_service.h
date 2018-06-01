//
// Created by guser on 5/23/18.
//

#ifndef RADIO_STREAMINGSERVICE_H
#define RADIO_STREAMINGSERVICE_H

#include <boost/circular_buffer.hpp>
#include <common/packet_dto.h>
#include "server_options.h"

class StreamingService {
private:
    int stream_sock;
    boost::circular_buffer<Packet> buffer;
    std::string mcast_address;
    int packet_id = 0;
    uint64_t session_id;
    uint16_t data_port;
    int fifo_size;
    int packet_size;

    int input_fd;
    int diag_fd;
public:
    void setDiag_fd(int diag_fd);

public:
    void setInput_fd(int input_fd);

public:
    virtual ~StreamingService();

    StreamingService(ServerOptions serverOptions);
    void setup();

    void start();


};


#endif //RADIO_STREAMINGSERVICE_H
