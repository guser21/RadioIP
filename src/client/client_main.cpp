//
// Created by guser on 5/24/18.
//

#include <client/receiver_service.h>
#include <common/const.h>
#include <client/discover_service.h>
#include <common/safe_structures.h>
#include <client/client_options.h>
#include <client/retransmission_service.h>

int main(int argc, char *argv[]) {
    ClientOptionsParser parser;
    auto clientOptions = parser.parse(argc, argv);

    DiscoverService discoverService(clientOptions.discover_addr, clientOptions.ctrl_port, clientOptions.rtime);
    UIService uiService(clientOptions.ui_port);
    RetransmissionService retransmissionService(clientOptions.rtime);
    clientOptions.buffer_size = 16;

    ReceiverService receiverService(discoverService, uiService,retransmissionService, clientOptions);
    receiverService.start();

    return 0;
}