//
// Created by guser on 5/28/18.
//

#include <client/buffer.h>

Buffer::Buffer(int size) {
    data = new char[size];
    byte_id = new int[size];
    clean();
}

Buffer::~Buffer() {
    delete data;
    delete byte_id;
}

double Buffer::load() {
    return busy / size;
}

//TODO test Non trivial code
void Buffer::push(char *buff, int buff_size, int first_byte) {
    int diff = 0;
    if (first_byte <= byte_id[read_index]) return;

    if (byte_id[read_index] != -1) {
        diff = first_byte - byte_id[read_index];
    }
    int from = (read_index + diff) % size;
    int index;
    for (int i = 0; i < buff_size; ++i) {
        index = (i + from) % size;

        data[index] = buff[i];
        byte_id[index] = first_byte + i;
    }
}

void Buffer::clean() {
    for (int i = 0; i < size; ++i) {
        data[i] = 0;
        byte_id[i] = -1;
    }
    last_read = -1;
}

std::pair<int, char *> Buffer::read() {
    int readable = 1;

    if (last_read != -1 && (last_read != byte_id[read_index] - 1)) {
        return std::pair<int, char *>(-1, nullptr);
    }

    for (int i = read_index; i < size - 1; ++i) {
        if (byte_id[i] == byte_id[i + 1] - 1)
            readable++;
        else
            break;
    }
    return std::pair<int, char *>(readable, &data[read_index]);
};

void Buffer::commit_read(int read_count) {
    if (read_count == 0) return;

    read_index += read_count;
    last_read = byte_id[read_index - 1];
    read_index = (read_index % size);
}