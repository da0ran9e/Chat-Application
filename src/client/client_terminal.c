#include "../../include/client/feature.h"

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

    if (current_index < index) {
        fprintf(stderr, "Error: Index %d is out of bounds\n", index);
        exit(EXIT_FAILURE);
    }
}

int main(){
    // Get the local IP address
    char ipBuffer[INET_ADDRSTRLEN];
    getLocalIPAddress(ipBuffer, sizeof(ipBuffer), 1);
    printf("Client LAN: %s\n", ipBuffer);

    int clientPort;
    printf("Connect to which port?( Enter a number ) ");
    scanf("%d", &clientPort);

    run_client(ipBuffer,clientPort);

    void showFeatures();
}