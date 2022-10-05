#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <sys/param.h>
#include <iostream>

#include "ping/ping_manager.h"
#include "ping/ping_packet.h"

namespace
{
    std::function<void()> generateReportAndExit;
    void signalHandler(int signal) { generateReportAndExit(); }
} // namespace

int main(int argc, char const *argv[])
{
    /* code */

    std::string destination;
    if (argc != 2)
    {
        printf("Invalid argument. Usage: ping <hostaname>\n");
        exit(1);
    }
    destination = std::string(argv[1]);

    PingManager pingManager = PingManager(destination);

    generateReportAndExit = [&]()
    {
        pingManager.generateReport();
        exit(0);
    };

    signal(SIGINT, signalHandler);
    pingManager.doPing();

    while (1)
    {
        /* code */
    }

    return 0;
}
