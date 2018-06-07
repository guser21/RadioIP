//
// Created by guser on 6/4/18.
//

#include <client/retransmission_service.h>
#include <common/const.h>
#include <algorithm>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;
using Ms = std::chrono::milliseconds;
template<class Duration>
using TimePoint = std::chrono::time_point<Clock, Duration>;

void RetransmissionService::add_request(uint64_t from, uint64_t to, int psize) {
    //every new addition should be served immediately
    //they should get different yet small times to be called before everyone
    std::unique_lock<std::mutex> lock(mutex);
    std::vector<uint64_t> requests;
    for (auto i = from; i < to; i += psize) {
        requests.push_back(i);
    }
    static int small_id = 0;
    small_id++;
    retransmissionQueue[small_id] = requests;
    cond.notify_one();
}

void RetransmissionService::notify(uint64_t id) {
    //naive implmentation but anyway
    // there are not going to more than 200 packets to retransmit
    std::unique_lock<std::mutex> lock(mutex);
    uint64_t key_to_map = 0;
    bool to_del = false;

    for (auto &elems:retransmissionQueue) {
        auto &vec = elems.second;
        auto iterator = std::find(vec.begin(), vec.end(), id);
        if (iterator != vec.end()) {
            vec.erase(iterator);
            if (vec.empty()) {
                to_del = true;
                key_to_map = elems.first;
            }
            break;
        }
    }
    if (to_del) {
        retransmissionQueue.erase(key_to_map);
    }
}

static std::string serialize_to_msg(std::vector<uint64_t> &pack_ids) {
    std::string msg(RETRY_MSG);
    msg += " ";
    for (int i = 0; i < pack_ids.size() - 1; ++i) {
        msg += std::to_string(pack_ids[i]);
        msg += ",";
    }
    msg += std::to_string(pack_ids[pack_ids.size() - 1]);
    msg+="\n";
    return msg;
}

static uint64_t get_now_mill() {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    auto millis_now = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
    return millis_now;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void RetransmissionService::start() {
    std::thread([&] {
        auto max_point = std::chrono::time_point<Clock, Ms>::max();
        auto stall = std::chrono::time_point_cast<std::chrono::milliseconds>(max_point).time_since_epoch().count();
        auto next_read = stall;

        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            auto until = TimePoint<Ms>(Ms(next_read));

            cond.wait_until(lock, until,
                            [&]() { return is_restarted || !retransmissionQueue.empty(); });


            if (is_restarted) {
                is_restarted = false;
                next_read = stall;
            } else {
                auto millis_now = get_now_mill();
                auto smallest = retransmissionQueue.begin();
                //TODO test
                while (!retransmissionQueue.empty() && smallest->first <= millis_now) {
                    auto msg = serialize_to_msg(smallest->second);
                    //TODO may block
                    auto sent_data = sendto(retr_socket, msg.c_str(), msg.size(), 0,
                                            reinterpret_cast<const sockaddr *>(&server_addr),
                                            sizeof(server_addr));

                    //TODO Add error handling
                    std::cerr << "sending request " << msg << std::endl;
                    if (sent_data < 0) logerr("couldn't send retransmission");

                    uint64_t next_update = get_now_mill() + rtime;
                    retransmissionQueue[next_update] = smallest->second;

                    retransmissionQueue.erase(smallest);
                    smallest = retransmissionQueue.begin();
                }
                next_read = retransmissionQueue.begin()->first;
            }
        }
    }).detach();
}

#pragma clang diagnostic pop

void RetransmissionService::set_server_addr(struct sockaddr_in addr) {
    std::unique_lock<std::mutex> lock(mutex);
    server_addr = addr;
}

void RetransmissionService::restart(struct sockaddr_in addr) {
    std::unique_lock<std::mutex> lock(mutex);
    server_addr = addr;
    is_restarted = true;
    retransmissionQueue.clear();
    cond.notify_one();
}

RetransmissionService::RetransmissionService(uint64_t rtime) : rtime(rtime) {
    retr_socket = socket(AF_INET, SOCK_DGRAM, 0);


    //not the best solution but ensures responsiveness
    timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500 * 1000;
    if (setsockopt(retr_socket, SOL_SOCKET, SO_SNDTIMEO, (void *) &timeout, sizeof(timeout)) < 0)
        syserr("setsockopt failed");


    if (retr_socket < 0) logerr("error in retransmission service socket");
    start();
}

