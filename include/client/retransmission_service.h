//
// Created by guser on 6/4/18.
//

#ifndef RADIO_RETRANSMISSION_SERVICE_H
#define RADIO_RETRANSMISSION_SERVICE_H

#include <common/safe_structures.h>
#include <netinet/in.h>
#include <poll.h>
#include <common/err.h>
#include <thread>
#include <zconf.h>
#include <fcntl.h>
#include <poll.h>
#include <atomic>


class RetransmissionService {
private:

    std::mutex mutex;
    std::condition_variable cond;
    std::atomic<bool> is_restarted = ATOMIC_VAR_INIT(false);
    std::map<uint64_t, std::vector<uint64_t>> retransmissionQueue;
    struct sockaddr_in server_addr{};
    int retr_socket;
    uint64_t rtime;
public:
    explicit RetransmissionService(uint64_t rtime);

    void add_request(uint64_t from, uint64_t to, int psize);

    void notify(uint64_t id);

    void set_server_addr(struct sockaddr_in server_addr);

    void restart(struct sockaddr_in server_addr);

    void start();
};

#endif //RADIO_RETRANSMISSION_SERVICE_H
