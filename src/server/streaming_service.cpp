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
    Packet *packet = static_cast<Packet *>(malloc(sizeof(Packet) + packet_size + 1));

    ssize_t read_chars, packed_chars = 0;

    bzero(read_buffer, sizeof(read_buffer));

    while ((read_chars = read(input_fd, read_buffer, sizeof(read_buffer))) > 0) {

        for (int j = 0; j < read_chars; ++j) {
            packet->audio_data[packed_chars] = read_buffer[j];
            packed_chars++;
            if (packed_chars == packet_size) {
                packet->session_id = this->session_id;
                packet->first_byte_num = packet_id * packet_size;
                //TODO with htons
                write(stream_sock, reinterpret_cast<char *>(packet), packet_size + 16); //may be worst idea ever
//                write(STDERR_FILENO, packet->audio_data, packet_size); //may be worst idea ever
//                fprintf(stderr, "written to socket %s", packet->audio_data);
//                buffer->push_back(packet);

                packed_chars = 0;
                packet_id++;
            }
        }
    }
    free(packet);
}

StreamingService::~StreamingService() = default;


StreamingService::StreamingService(ServerOptions serverOptions) {
    this->packet_size = serverOptions.packet_size;
    this->fifo_size = serverOptions.fifo_size;
    this->data_port = serverOptions.data_port;
    this->mcast_address = serverOptions.mcast_addr;

    this->input_fd = STDIN_FILENO;

    buffer.set_capacity(static_cast<unsigned long>(serverOptions.fifo_size / serverOptions.packet_size));
    session_id = static_cast<uint64_t>(std::time(nullptr));
}


#pragma clang diagnostic pop