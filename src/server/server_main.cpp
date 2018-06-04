#include <common/const.h>
#include <server/streaming_service.h>
#include <server/control_daemon.h>
#include <server/server_options.h>
#include <iostream>
#include <common/safe_structures.h>
#include <server/retransmission_daemon.h>

int main(int argc, char *argv[]) {
    ServerOptionParser parser;

    auto server_options = parser.parse(argc, argv);

    SafeBuffer buffer(server_options.fifo_size / server_options.packet_size, server_options.packet_size);
    SetMutex<uint64_t> retr_requests;

//    server_options.packet_size=8;
    StreamingService streamingService(server_options, &buffer);
    ControlDaemon controlDaemon(server_options, &retr_requests);

    controlDaemon.setup();
    streamingService.setup();

    RetransmissionDaemon retransmission(&retr_requests,
                                                streamingService.get_streaming_socket(),
                                                server_options.rtime, &buffer);

    retransmission.start();
    controlDaemon.start();
    streamingService.start();


    return 0;
}

