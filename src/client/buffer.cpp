//
// Created by guser on 5/28/18.
//

#include <client/buffer.h>

Buffer::Buffer(int size) : size(size) {
    data = new char[size];
    byteId = new __int128[size];
    clean();
}

Buffer::~Buffer() {
    delete data;
    delete byteId;
}

__int128 Buffer::get_lastId() {
    return last_byteId;
}

//TODO test Non trivial code
void Buffer::push(const char *buff, int buff_size, int first_byte) {
    uint64_t diff = 0;

    if (first_byte <= byteId[read_index]) return;

    if (byteId[read_index] != -1) {
        diff = first_byte - byteId[read_index];
    }
    uint64_t from = (read_index + diff) % size;
    uint64_t index;
    for (int i = 0; i < buff_size; ++i) {
        index = (i + from) % size;

        data[index] = buff[i];
        byteId[index] = first_byte + i;
    }
    last_byteId = first_byte + buff_size - 1;
}

void Buffer::clean() {
    read_index = 0;
    last_read = -1;
    for (int i = 0; i < size; ++i) {
        data[i] = 0;
        byteId[i] = -1;
    }
}

std::pair<int, char *> Buffer::read() {
    int readable = 1;

    if (last_read != -1 && (last_read != byteId[read_index] - 1)) {
        return std::pair<int, char *>(0, nullptr);
    }

    for (int i = read_index; i < size - 1; ++i) {
        if (byteId[i] == byteId[i + 1] - 1)
            readable++;
        else
            break;
    }
    return std::pair<int, char *>(readable, &data[read_index]);
};

void Buffer::commit_read(int read_count) {
    if (read_count == 0) return;
    read_index += read_count;
    last_read = byteId[read_index - 1];
    read_index = (read_index % size);
}