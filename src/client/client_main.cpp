//
// Created by guser on 5/24/18.
//

#include <client/reciever_service.h>
#include <common/const.h>
#include <client/discover_service.h>
#include <common/safe_structures.h>
#include <client/client_options.h>

int main(int argc, char *argv[]) {
    ClientOptionsParser parser;
    auto clientOptions = parser.parse(argc, argv);

    DiscoverService discoverService(clientOptions.discover_addr, clientOptions.ctrl_port, clientOptions.rtime);
    UIService uiService(clientOptions.ui_port);
    
//    clientOptions.buffer_size = 16;

    ReceiverService receiverService(discoverService, uiService, clientOptions);
    receiverService.start();

    return 0;
}