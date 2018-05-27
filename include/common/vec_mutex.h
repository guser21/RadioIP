//
// Created by guser on 5/26/18.
//

#ifndef RADIO_THREADSAFE_LIST_H
#define RADIO_THREADSAFE_LIST_H

#include <mutex>
#include <vector>
#include <condition_variable>

template<typename T>
class VecMutex {
public:
    std::vector<T> vector;
    std::mutex mut;
    std::condition_variable cond;

    T blocking_peek() {
        std::unique_lock<std::mutex> lock(mut);
        cond.wait(lock, [&] { return !vector.empty(); });
        return vector[0];

    }
};

#endif //RADIO_THREADSAFE_LIST_H
