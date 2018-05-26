//
// Created by guser on 5/24/18.
//

#include <client/reciever_service.h>
#include <cstdio>
#include <zconf.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <utility>
#include <common/err.h>
#include <common/const.h>
#include <common/packet_dto.h>

void ReceiverService::start() {

    int packed_bytes = 0;
    ssize_t read_bytes;
    Packet *packet;
    char read_buffer[sizeof(Packet)], packet_buffer[sizeof(Packet)];

    while ((read_bytes = read(stream_sock, read_buffer, sizeof(read_buffer))) > 0) {

        for (int j = 0; j < read_bytes; ++j) {
            packet_buffer[packed_bytes] = read_buffer[j];
            packed_bytes++;
            if (packed_bytes == sizeof(Packet)) {
                packet = reinterpret_cast<Packet *> (packet_buffer);
                printf("session_id %lu \n", packet->session_id);
                printf("first_byte %lu\n", packet->first_byte_num);
                write(STDOUT_FILENO, packet->audio_data,PSIZE);
                printf("\n");

                packed_bytes = 0;
            }
        }
    }


}

void ReceiverService::setup() {

    struct sockaddr_in local_address{};
    struct ip_mreq ip_mreq{};


    stream_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (stream_sock < 0)syserr("socket");

    ip_mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (inet_aton(mcast_address.c_str(), &ip_mreq.imr_multiaddr) == 0)
        syserr("inet_aton");

    if (setsockopt(stream_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *) &ip_mreq, sizeof ip_mreq) < 0)
        syserr("setsockopt");

    local_address.sin_family = AF_INET;
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    local_address.sin_port = htons(port);

//    connect(stream_sock, (struct sockaddr *) &local_address, sizeof(local_address));
    if (bind(stream_sock, (struct sockaddr *) &local_address, sizeof local_address) < 0)
        syserr("bind");
}

ReceiverService::ReceiverService(std::string mcast_address, int port)
        : mcast_address(std::move(mcast_address)), port(port) {}