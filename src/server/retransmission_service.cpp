//
// Created by guser on 5/31/18.
//



#include <server/retransmission_service.h>
#include <zconf.h>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void RetransmissionService::start() {
    while (true) {
        usleep(rtime);
        std::vector<uint64_t> reqs;
        {
            std::unique_lock<std::mutex> lock(retr_req->mut);
            reqs.reserve(retr_req->elems.size());
            std::copy(retr_req->elems.begin(), retr_req->elems.end(), reqs.begin());
            retr_req->elems.clear();
        }
        for (auto el:reqs) {
            auto packet = safeBuffer->get(el);
            if (packet.length() != 0)
                write(socket, packet.c_str(), packet.size());
        }
    }

}

#pragma clang diagnostic pop