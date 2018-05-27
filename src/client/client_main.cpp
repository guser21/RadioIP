//
// Created by guser on 5/24/18.
//

#include <client/reciever_service.h>
#include <common/const.h>
#include <client/discover_service.h>
#include <common/vec_mutex.h>

int main() {

    VecMutex<Station> vecMutex{};

    DiscoverService discoverService(DISCOVER_ADDR, CTRL_PORT, RTIME, &vecMutex);
    discoverService.setup();
    discoverService.start();

    ReceiverService receiverService(discoverService.get_disc_sock());
    receiverService.start();

    return 0;
}