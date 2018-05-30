//
// Created by guser on 5/28/18.
//

#ifndef RADIO_BUFFER_H
#define RADIO_BUFFER_H

#include <utility>
#include <stdint-gcc.h>
/**
 * character and byte index
 * */
//TODO unit test
class Buffer {
private:
    int size;
    int read_index = 0;
    __int128 last_read = -1;
    __int128 last_byteId=-1;
    char *data;
    __int128 *byteId;
public:

    explicit Buffer(int size);

    ~Buffer();

    __int128 get_lastId();

    void push(const char arr[], int size, int first_byte);

    void clean();

    std::pair<int, char *> read();

    void commit_read(int read_count);
};

#endif //RADIO_BUFFER_H
