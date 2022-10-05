#include <netdb.h>
#include <unistd.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <iostream>
#include <chrono>

#include "ping_manager.h"
#include "../utils.h"

PingManager::PingManager() {}

PingManager::PingManager(std::string destination) : _runPing(false)
{
    _destination = destination;
}

std::string PingManager::getRandomData(size_t len, char c)
{
    std::string result = std::string(len, c);
    return result;
}

void PingManager::doPing()
{
    _runPing = true;
    _destinationAddr = Utils::getDestinationAddr(_destination);

    this->setDestinationIP();

    printf("Ping %s (%s): %lu data bytes\n", _destination.c_str(), _destinationIP.c_str(), sizeof(PingPacket));

    this->run();
}

void PingManager::generateReport()
{
    _runPing = false;
    std::cout << "Generate report and exit, source:"
              << htonl(((sockaddr_in *)&_destinationAddr)->sin_addr.s_addr) << " \n";
    exit(0);
}

void PingManager::setDestinationIP()
{
    void *addr;

    if (_destinationAddr.ss_family == AF_INET)
    { // IPv4
        addr = &(((struct sockaddr_in *)&_destinationAddr)->sin_addr);
    }
    else
    { // IPv6
        addr = &(((struct sockaddr_in6 *)&_destinationAddr)->sin6_addr);
    }
    // convert the IP to a string and print it:
    inet_ntop(_destinationAddr.ss_family, addr, &_destinationIP[0], sizeof _destinationIP);
}

int PingManager::getSocketFD(bool isIPV6)
{
    // TODO: add socket options
    int fd = 0;

    int family = isIPV6 ? AF_INET6 : AF_INET;
    int proto = isIPV6 ? IPPROTO_ICMPV6 : IPPROTO_ICMP;

    if (getuid())
    {
        fd = socket(family, SOCK_DGRAM, proto); // Standard user
    }
    else
    {
        fd = socket(family, SOCK_RAW, proto); // Root user
    }

    return fd;
}

void PingManager::run()
{
    int sockFD = this->getSocketFD();
    struct PingPacket pingPKT, *respPKT = nullptr;
    struct sockaddr receiveAddr;
    int seqNumber = -1;
    int numbBytes = 0;
    char buf[MAX_RECV_DATA_LENGTH];

    std::string msg = "";
    socklen_t addrLen = 0;
    struct ip *ipHeader = nullptr;
    size_t ipHeaderLen = 0;
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> start;

    while (_runPing)
    {
        /* code */
        bzero(&pingPKT, sizeof pingPKT);
        bzero(&buf, sizeof buf);
        msg = "";
        addrLen = 0;
        ipHeaderLen = 0;
        ipHeader = nullptr;
        respPKT = nullptr;

        pingPKT.type = ICMP_ECHO;
        pingPKT.code = 0;
        pingPKT.identifier = getpid();
        pingPKT.seqNumber = ++seqNumber;
        msg = this->getRandomData(sizeof(pingPKT.msg), 'a' + pingPKT.seqNumber);
        strcpy(pingPKT.msg, msg.c_str());
        pingPKT.checksum = Utils::getICMPCheckSum((uint16_t *)&pingPKT, sizeof pingPKT);

        start = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
        if ((numbBytes = sendto(sockFD, &pingPKT, sizeof pingPKT, 0, (struct sockaddr *)(&_destinationAddr), sizeof _destinationAddr)) == -1)
        {
            std::cout << "Error while sending the data \n";
            exit(1);
        }

        addrLen = sizeof(receiveAddr);
        if ((numbBytes = recvfrom(sockFD, &buf, sizeof buf, 0, (struct sockaddr *)(&receiveAddr), &addrLen)) == -1)
        {
            std::cout << "Error while receiving the data \n";
            exit(1);
        }
        auto finish = std::chrono::system_clock::now();

        ipHeader = (struct ip *)(buf);
        ipHeaderLen = ipHeader->ip_hl << 2;

        respPKT = (struct PingPacket *)(buf + ipHeaderLen);

        if (respPKT->type != ICMP_ECHOREPLY)
        {
            std::cout << "Type is not echo reply \n";
            exit(1);
        }

        if (respPKT->code != 0)
        {
            std::cout << "Code is not 0 \n";
            exit(1);
        }

        if (strcmp(msg.c_str(), respPKT->msg))
        {
            std::cout << "Received message mismatch \n";
            exit(1);
        }

        if (Utils::getICMPCheckSum((uint16_t *)respPKT, sizeof *respPKT) != 0)
        {
            std::cout << "Checksum of received packet is not correct \n";
            exit(1);
        }

        printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%f ms\n", numbBytes, _destinationIP.c_str(), respPKT->seqNumber, ipHeader->ip_ttl, (finish - start).count() / 1000.0);

        sleep(1);
    }
}

PingManager::~PingManager() {}