#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "utils.h"

sockaddr_storage Utils::getDestinationAddr(std::string domain)
{
    int status;
    struct addrinfo hints;
    struct addrinfo *servinfo, *p; // will point to the results
    // TODO: Change this to pointer
    static struct sockaddr_storage addr_storage;
    std::string ipstr = "", ipver = "";

    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    // TODO: Add ipv6
    hints.ai_family = AF_INET;       // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    if ((status = getaddrinfo(domain.c_str(), NULL, &hints, &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        void *addr;

        memmove(&addr_storage, (struct sockaddr_storage *)p->ai_addr, sizeof(addr_storage));

        // get the pointer to the addr_storage itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET)
        { // IPv4
            addr = &(((struct sockaddr_in *)&addr_storage)->sin_addr);
            ipver = "IPv4";
        }
        else
        { // IPv6
            addr = &(((struct sockaddr_in6 *)&addr_storage)->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, &ipstr[0], sizeof ipstr);
        break;
    }

    printf("Domain:%s IP:%s IPv:%s\n", domain.c_str(), ipstr.c_str(), ipver.c_str());

    if (servinfo)
        freeaddrinfo(servinfo); // free the linked-list

    return addr_storage;
}

uint16_t Utils::getICMPCheckSum(uint16_t *data, int length)
{

    assert(length >= 0);

    uint16_t result = 0;
    uint32_t sum = 0;
    uint16_t odd_byte;

    while (length > 1)
    {
        /* code */
        sum += *data++;
        length -= 2;
    }

    if (length == 1)
    {
        *(uint8_t *)(&odd_byte) = *(uint8_t *)data;
        sum += odd_byte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    result = ~sum;

    return result;
}