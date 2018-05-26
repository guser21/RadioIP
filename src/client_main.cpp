//
// Created by guser on 5/24/18.
//

#include <client/reciever_service.h>
#include <common/const.h>

int main() {
    ReceiverService recieverService(MCAST_ADDR, DATA_PORT);
    recieverService.setup();
    recieverService.start();

    return 0;
}