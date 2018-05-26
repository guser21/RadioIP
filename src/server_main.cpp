#include <common/const.h>
#include <server/streaming_service.h>
#include <server/control_daemon.h>

int main(int argc, char *argv[]) {
    StreamingService streamingService(STDIN_FILENO, MCAST_ADDR, DATA_PORT);
    ControlDaemon controlDaemon(CTRL_PORT, MCAST_ADDR, DATA_PORT, STATION_NAME);

    controlDaemon.setup();
    controlDaemon.start();

    streamingService.setup();
    streamingService.start();


    return 0;
}

