//
// Created by guser on 5/23/18.
//

#ifndef RADIO_STREAMINGSERVICE_H
#define RADIO_STREAMINGSERVICE_H

#include <boost/circular_buffer.hpp>
#include <common/packet_dto.h>
#include <common/vec_mutex.h>
#include "server_options.h"

class StreamingService {
private:
    int stream_sock;
    std::string mcast_address;
    int packet_id = 0;
    uint64_t session_id;
    uint16_t data_port;
    int fifo_size;
    int packet_size;

    int input_fd;
    int diag_fd;

    SetMutex<uint64_t> *retr_requests;
    SafeBuffer* buffer;
public:
    void setDiag_fd(int diag_fd);

    void setInput_fd(int input_fd);

    virtual ~StreamingService();

    StreamingService(ServerOptions serverOptions, SetMutex<uint64_t> *retr_requests);

    void setup();

    void start();


};


#endif //RADIO_STREAMINGSERVICE_H
