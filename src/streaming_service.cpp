//
// Created by guser on 5/23/18.
//

#include <sys/socket.h>
#include <common/err.h>
#include <server/streaming_service.h>
#include <common/const.h>
#include <netinet/in.h>
#include <strings.h>
#include <arpa/inet.h>
#include <time.h>
#include <ctime>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void StreamingService::setup() {
    struct sockaddr_in remote_address{};

    stream_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (stream_sock < 0) syserr("socket");

    int optval = 1;
    if (setsockopt(stream_sock, SOL_SOCKET, SO_BROADCAST, (void *) &optval, sizeof optval) < 0)
        syserr("setsockopt broadcast");

    optval = TTL_VALUE;
    if (setsockopt(stream_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &optval, sizeof optval) < 0)
        syserr("setsockopt multicast ttl");

    bzero(&remote_address, sizeof(remote_address));
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(port);

    if (inet_aton(mcast_address.c_str(), &remote_address.sin_addr) == 0)
        syserr("inet_aton");

    //nothing to read from multicast address
    //address is bound to socket
    if (connect(stream_sock, (struct sockaddr *) &remote_address, sizeof remote_address) < 0)
        syserr("connect");

}

void StreamingService::start() {
    char read_buffer[PSIZE];
    Packet packet{};
    ssize_t read_chars, packed_chars = 0;

    bzero(read_buffer, sizeof(read_buffer));

    while ((read_chars = read(input_fd, read_buffer, sizeof(read_buffer))) > 0) {

        for (int j = 0; j < read_chars; ++j) {
            packet.audio_data[packed_chars] = read_buffer[j];
            packed_chars++;
            if (packed_chars == PSIZE) {
                packet.session_id = this->session_id;
                packet.first_byte_num = packet_id * PSIZE;
                write(stream_sock, reinterpret_cast<char *>(&packet), sizeof(packet)); //may be worst idea ever

                buffer.push_back(packet);

                packed_chars = 0;
                packet_id++;
            }
        }
    }
}

StreamingService::~StreamingService() = default;

StreamingService::StreamingService(int input_fd, const std::string &mcast_address, int port) :
        input_fd(input_fd), mcast_address(mcast_address), port(port) {
    buffer.set_capacity(BSIZE / PSIZE);
    session_id = static_cast<uint64_t>(std::time(nullptr));
}


#pragma clang diagnostic pop