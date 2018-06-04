//
// Created by guser on 5/31/18.
//

#ifndef RADIO_RETRANSMISSION_SERVICE_H
#define RADIO_RETRANSMISSION_SERVICE_H

#include <common/safe_structures.h>

class RetransmissionDaemon {
private:
    SetMutex<uint64_t > *retr_req;
    int socket;
    int rtime;
    SafeBuffer* safeBuffer;

public:
    RetransmissionDaemon(SetMutex<uint64_t> *retr_req, int socket, int rtime, SafeBuffer *safeBuffer);

    void start();

};

#endif //RADIO_RETRANSMISSION_SERVICE_H
