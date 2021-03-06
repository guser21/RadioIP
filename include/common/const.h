
//
// Created by guser on 5/21/18.
//

#ifndef RADIO_CONST_H
#define RADIO_CONST_H

#define DATA_PORT 21501
#define CTRL_PORT 31501
#define DISCOVER_ADDR "255.255.255.255"
#define PSIZE 1024
#define BSIZE 524288
#define FSIZE 131072
#define TTL_VALUE 60
#define READ_BUFFER_SIZE 1000
#define CTRL_BUFFER_SIZE 300
#define LOAD_FACTOR 0.75
#define MAX_NAME_SIZE 100
#define LOOKUP_MSG "ZERO_SEVEN_COME_IN\n"
#define REPLY_MSG "BOREWICZ_HERE"
#define RETRY_MSG "LOUDER_PLEASE"
#define STATION_NAME "Unnamed Station"
#define RTIME_CLIENT 250
#define RTIME_SERVER 250
#define MAX_PORT 65535
#define UI_PORT 11501
#define DROP_TIMEOUT 20
#define QUEUE_LENGTH 5
#define MAX_UDP_SIZE (65535+1000)
#define TCP_READ_BUFFER 100
#define DISCOVER_REPEAT 5
#define LINE "------------------------------------------------------------------------"
#define CLIENT_NAME "  Radio Active"
const char UP_KEY[] = {27, 91, 65};
const char DOWN_KEY[] = {27, 91, 66};
const char CLEAR[] = {0x1B, 'c', '\0'};
const char LINEMODE[] = "\377\375\042\377\373\001";

#endif //RADIO_CONST_H
