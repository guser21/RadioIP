//
// Created by guser on 5/31/18.
//

#ifndef RADIO_RETRANSMISSION_SERVICE_H
#define RADIO_RETRANSMISSION_SERVICE_H

#include <common/vec_mutex.h>

class RetransmissionService {
private:
    SetMutex<uint64_t > *retr_req;
    int socket;
    uint64_t session_id;
    int rtime;
    SafeBuffer* safeBuffer;

public:
    void start();

};

#endif //RADIO_RETRANSMISSION_SERVICE_H
