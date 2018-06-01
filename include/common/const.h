//
// Created by guser on 5/21/18.
//

#ifndef RADIO_CONST_H
#define RADIO_CONST_H

#define DATA_PORT 21501
#define CTRL_PORT 31501
#define MCAST_ADDR "224.34.122.2"
#define DISCOVER_ADDR "255.255.255.255"
#define PSIZE 512
#define BSIZE 65536
#define FSIZE 131072
#define TTL_VALUE 60
#define READ_BUFFER_SIZE 1000
#define CTRL_BUFFER_SIZE 300
#define LOAD_FACTOR 0.75
#define MAX_NAME_SIZE 100
#define LOOKUP_MSG "ZERO_SEVEN_COME_IN"
#define REPLY_MSG "BOREWICZ_HERE"
#define STATION_NAME "Radio Erewań"//TODO
#define RTIME 3
#define RTIME_CLIENT 3
#define RTIME_SERVER 250
#define MAX_PORT 65535
#define UI_PORT 11501
#define DROP_TIMEOUT 20
#define QUEUE_LENGTH 5
#define MAX_UDP_SIZE (65535+1000)
const char UP_KEY[] = {27, 91, 65};
const char DOWN_KEY[] = {27, 91, 66};
const char ENTER[] = {13};
const char CLEAR[] = {0x1B, 'c', '\0'};
const char LINEMODE[] = "\377\375\042\377\373\001";


#endif //RADIO_CONST_H
