#pragma once

#define PING_PACKET_HEADER_LENGTH 64
#define PING_PACKET_DATA_LENGTH 64

struct PingPacket
{
    /* data */
    uint8_t type;
    uint8_t code; // = 0 for echo and echo reply
    uint16_t checksum;
    uint16_t identifier;
    uint16_t seqNumber;
    // uint64_t msg;
    char msg[PING_PACKET_DATA_LENGTH];
};


#include <netinet/ip_icmp.h>
// Define the Packet Constants
// ping packet size
#define PING_PKT_S 64

// struct PingPacket
// {
//     struct icmp hdr;
//     char msg[PING_PKT_S-sizeof(struct icmp)];
// };
 