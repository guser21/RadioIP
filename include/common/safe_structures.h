//
// Created by guser on 5/26/18.
//

#ifndef RADIO_THREADSAFE_LIST_H
#define RADIO_THREADSAFE_LIST_H

#include <mutex>
#include <vector>
#include <condition_variable>
#include <set>
#include <map>

template<typename T>
struct SetMutex {
    std::set<T> elems;
    std::mutex mut;
};

class SafeBuffer {
private:
    std::mutex mut;
    std::map<uint64_t, std::string> map;
    int size;
    int psize;
public:

    SafeBuffer(int size,int psize) : size(size),psize(psize) {}
    SafeBuffer(){};
    void push(uint64_t p_num, std::string str) {
        std::unique_lock<std::mutex> lock(mut);
        map.emplace(p_num, str);
        map.erase(p_num - (size * psize));
    }

    std::string get(uint64_t p_num) {
        std::unique_lock<std::mutex> lock(mut);
        auto it = map.find(p_num);
        if (it != map.end()) {
            return it->second;
        }
        return std::string("");
    }
};

#endif //RADIO_THREADSAFE_LIST_H
