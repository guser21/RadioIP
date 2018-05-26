//
// Created by guser on 5/24/18.
//

#ifndef RADIO_PACKET_DTO_H
#define RADIO_PACKET_DTO_H

#include <cstdint>
#include <common/const.h>
struct Packet {
    uint64_t session_id;
    uint64_t first_byte_num;
    char audio_data[PSIZE];
};

#endif //RADIO_PACKET_DTO_H
