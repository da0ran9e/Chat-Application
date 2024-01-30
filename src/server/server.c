
/* Get the system IP address */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>

void getLocalIPAddress(char* ipBuffer, size_t bufferSize, int index) {
    struct ifaddrs* ifaddr, * ifa;
    int current_index = 0;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            if (current_index == index) {
                struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
                inet_ntop(AF_INET, &(addr->sin_addr), ipBuffer, bufferSize);
                break;  // Found the desired index
            }
            current_index++;
        }
    }

    freeifaddrs(ifaddr);

    if (current_index <= index) {
        fprintf(stderr, "Error: Index %d is out of bounds\n", index);
        exit(EXIT_FAILURE);
    }
}

/* Setup Server */

int main() {
    // Get the local IP address
    char ipBuffer[INET_ADDRSTRLEN];
    getLocalIPAddress(ipBuffer, sizeof(ipBuffer), 1);
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