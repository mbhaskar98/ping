#pragma once

#include <iostream>
#include <stdio.h>
#include <sys/socket.h>

namespace Utils
{
    sockaddr_storage getDestinationAddr(std::string domain);
    uint16_t getICMPCheckSum(uint16_t *data, int length);
}
