//
// Created by guser on 5/28/18.
//

#ifndef RADIO_BUFFER_H
#define RADIO_BUFFER_H

#include <utility>
/**
 * character and byte index
 * */
//TODO unit test
class Buffer {
private:
    int size;
    int read_index = 0;
    int busy = 0;
    char *data;
    int *byte_id;
    int last_read=-1;
public:
    explicit Buffer(int size);

    ~Buffer();

    double load();

    void push(char arr[], int size, int first_byte);

    void clean();

    std::pair<int, char *> read();

    void commit_read(int read_count);
};

#endif //RADIO_BUFFER_H
