//
// Created by guser on 5/31/18.
//



#include <server/retransmission_daemon.h>
#include <unistd.h>
#include <thread>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void RetransmissionDaemon::start() {
    std::thread([&] {
        while (true) {
            std::this_thread::sleep_for(std::chrono::milliseconds(rtime));
            std::vector<uint64_t> reqs;
            {
                std::unique_lock<std::mutex> lock(retr_req->mut);
                reqs.reserve(retr_req->elems.size());
                std::copy(retr_req->elems.begin(), retr_req->elems.end(), std::back_inserter(reqs));
                retr_req->elems.clear();
            }
            for (auto el:reqs) {
                auto packet = safeBuffer->get(el);
                if (packet.length() != 0) {
                    //sockets are thread safe
                    write(socket, packet.c_str(), packet.size());
                }
            }
        }
    }).detach();
}

RetransmissionDaemon::RetransmissionDaemon(SetMutex<uint64_t> *retr_req, int socket, int rtime,
                                           SafeBuffer *safeBuffer) : retr_req(retr_req),
                                                                     socket(socket),
                                                                     rtime(rtime),
                                                                     safeBuffer(safeBuffer) {}

#pragma clang diagnostic pop