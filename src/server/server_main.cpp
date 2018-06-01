#include <common/const.h>
#include <server/streaming_service.h>
#include <server/control_daemon.h>
#include <server/server_options.h>
#include <iostream>

int main(int argc, char *argv[]) {
    ServerOptionParser parser;
    auto server_options = parser.parse(argc, argv);

//    server_options.packet_size=8;
    StreamingService streamingService(server_options);
    ControlDaemon controlDaemon(server_options);

    controlDaemon.setup();
    controlDaemon.start();

    streamingService.setup();
    streamingService.start();


    return 0;
}

