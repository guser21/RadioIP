//
// Created by guser on 5/24/18.
//

#include <client/reciever_service.h>
#include <common/const.h>
#include <client/discover_service.h>
#include <common/vec_mutex.h>
#include <client/client_options.h>

int main(int argc, char *argv[]) {
    ClientOptionsParser parser;
    auto clientOptions = parser.parse(argc, argv);

    DiscoverService discoverService(DISCOVER_ADDR, CTRL_PORT, RTIME);
    UIService uiService(UI_PORT);
    clientOptions.buffer_size = 8;
    ReceiverService receiverService(discoverService, uiService, clientOptions);
    receiverService.start();

    return 0;
}