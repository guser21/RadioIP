//
// Created by guser on 5/31/18.
//

#ifndef RADIO_RETRANSMISSION_SERVICE_H
#define RADIO_RETRANSMISSION_SERVICE_H

#include <common/vec_mutex.h>

class RetransmissionService {
private:
    VecMutex<int> *vecMutex;
public:


};

#endif //RADIO_RETRANSMISSION_SERVICE_H
