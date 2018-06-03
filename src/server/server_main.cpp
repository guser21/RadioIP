#include <common/const.h>
#include <server/streaming_service.h>
#include <server/control_daemon.h>
#include <server/server_options.h>
#include <iostream>
#include <common/vec_mutex.h>

int main(int argc, char *argv[]) {
    ServerOptionParser parser;
    auto server_options = parser.parse(argc, argv);

    SetMutex<uint64_t > retr_requests;
//    server_options.packet_size=8;
    StreamingService streamingService(server_options,&retr_requests);
    ControlDaemon controlDaemon(server_options,&retr_requests);

    controlDaemon.setup();
    controlDaemon.start();

    streamingService.setup();
    streamingService.start();


    return 0;
}

