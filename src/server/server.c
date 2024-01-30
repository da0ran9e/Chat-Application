
/* Get the system IP address */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>

void getLocalIPAddress(char* ipBuffer, size_t bufferSize) {
    struct ifaddrs* ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            inet_ntop(AF_INET, &(addr->sin_addr), ipBuffer, bufferSize);
            break;  // Assuming the first IPv4 address found is the one to use
        }
    }

    freeifaddrs(ifaddr);
}

/* Setup Server */

int main() {
    // Get the local IP address
    char ipBuffer[INET_ADDRSTRLEN];
    getLocalIPAddress(ipBuffer, sizeof(ipBuffer));
    printf("%s\n", ipBuffer);
    return 0;
}

/*
int main(int argc, char const *argv[]) {
    if (argc != 2) {
        printf("Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }


    int port = atoi(argv[1]);

    _s_init(port);
    return 0;
}
*/