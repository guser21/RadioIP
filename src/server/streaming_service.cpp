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
#include <ctime>
#include <iostream>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

void StreamingService::setup() {
    struct sockaddr_in remote_address{};

    stream_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (stream_sock < 0) syserr("socket");

    int optval = 1;
    if (setsockopt(stream_sock, SOL_SOCKET, SO_BROADCAST, (void *) &optval, sizeof optval) < 0)
        syserr("setsockopt broadcast");

    if (setsockopt(stream_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
        syserr("setsockopt reuseaddr in server");

    optval = TTL_VALUE;
    if (setsockopt(stream_sock, IPPROTO_IP, IP_MULTICAST_TTL, (void *) &optval, sizeof optval) < 0)
        syserr("setsockopt multicast ttl");

    bzero(&remote_address, sizeof(remote_address));
    remote_address.sin_family = AF_INET;
    remote_address.sin_port = htons(data_port);//why data_port?

    if (inet_aton(mcast_address.c_str(), &remote_address.sin_addr) == 0)
        syserr("inet_aton");

    //nothing to read from multicast address
    //address is bound to socket
    if (connect(stream_sock, (struct sockaddr *) &remote_address, sizeof remote_address) < 0)
        syserr("connect");


}

void StreamingService::start() {

    char read_buffer[packet_size];
    char *pack_mem = new char[sizeof(Packet) + packet_size + 1];

    auto packet = reinterpret_cast<Packet *> (pack_mem);
    ssize_t read_chars, packed_chars = 0;

    bzero(read_buffer, sizeof(read_buffer));
    int skip = 0;

    while ((read_chars = read(input_fd, read_buffer, sizeof(read_buffer))) > 0) {

        for (int j = 0; j < read_chars; ++j) {
            packet->audio_data[packed_chars] = read_buffer[j];
            packed_chars++;
            if (packed_chars == packet_size) {
                auto cur_pack_id = packet_id * packet_size;

                packet->session_id = htobe64(session_id);
                packet->first_byte_num = htobe64(cur_pack_id);

                const char *raw_packet = reinterpret_cast<char *>(packet);
                std::string current_packet;

                for (int i = 0; i < packet_size + 16; i++) {
                    current_packet += raw_packet[i];
                }

                if (skip % 10 != 0) {
                    write(stream_sock, current_packet.c_str(), current_packet.size());
                    std::cerr << "written to socket " << cur_pack_id << std::endl;
                }
                skip++;
                buffer->push(cur_pack_id, current_packet);

                packed_chars = 0;
                packet_id++;
            }
        }
    }
    delete pack_mem;
}


StreamingService::StreamingService(ServerOptions serverOptions, SafeBuffer *safeBuffer) : buffer(safeBuffer) {
    this->packet_size = serverOptions.packet_size;
    this->fifo_size = serverOptions.fifo_size;
    this->data_port = serverOptions.data_port;
    this->mcast_address = serverOptions.mcast_addr;

    this->input_fd = STDIN_FILENO;
    this->diag_fd = STDOUT_FILENO;

    session_id = static_cast<uint64_t>(std::time(nullptr));

}

void StreamingService::setInput_fd(int input_fd) {
    StreamingService::input_fd = input_fd;
}

void StreamingService::setDiag_fd(int diag_fd) {
    StreamingService::diag_fd = diag_fd;
}


#pragma clang diagnostic pop