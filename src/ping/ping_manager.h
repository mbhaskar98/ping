#pragma once

#include <iostream>
#include <sys/socket.h>

#include "ping_packet.h"

#define MAX_RECV_DATA_LENGTH 1024

class PingManager
{
public:
    PingManager();
    PingManager(std::string destination);
    ~PingManager();

    void doPing();
    void generateReport();
    std::string getRandomData(size_t len, char c);

private:
    std::string _destination, _destinationIP;
    struct sockaddr_storage _destinationAddr;
    bool _runPing;

    int getSocketFD(bool isIPV6 = false);
    void setDestinationIP();
    void run();
};
