//
// Created by guser on 6/4/18.
//

#ifndef RADIO_RETRANSMISSION_SERVICE_H
#define RADIO_RETRANSMISSION_SERVICE_H

#include <common/safe_structures.h>

class RetransmissionService {
private:
    std::mutex mutex;
    std::condition_variable cond;
    std::set<std::pair<uint64_t, std::vector<uint64_t,std::less(uint64_t) >>> retransmissionQueue();

public:
    void add_request(std::vector<uint64_t> requests);

    void notify(uint64_t id);

    void start();
};

#endif //RADIO_RETRANSMISSION_SERVICE_H
