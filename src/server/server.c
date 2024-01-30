
/* Get the system IP address */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>

void getIPAddress(char *ipBuffer, size_t bufferSize) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("WSAStartup failed");
        exit(EXIT_FAILURE);
    }

    char hostname[100];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        perror("gethostname failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo(hostname, NULL, &hints, &result) != 0) {
        perror("getaddrinfo failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    char ipstr[INET6_ADDRSTRLEN];
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        void* addr;
        if (ptr->ai_family == AF_INET) {
            struct sockaddr_in* ipv4 = (struct sockaddr_in*)ptr->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)ptr->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        inet_ntop(ptr->ai_family, addr, ipstr, sizeof(ipstr));
        strncpy(ipBuffer, ipstr, bufferSize);
        break; // Get the first address and break
    }

    freeaddrinfo(result);
    WSACleanup();
    struct ifaddrs* ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL || (ifa->ifa_flags & IFF_UP) == 0 || (ifa->ifa_flags & IFF_LOOPBACK) != 0)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            inet_ntop(AF_INET, &(addr->sin_addr), ipBuffer, bufferSize);
            break; // Get the first address and break
        }
    }

    freeifaddrs(ifaddr);
}

/* Setup Server */

int main() {
    char ipBuffer[INET_ADDRSTRLEN];
    getIPAddress(ipBuffer, sizeof(ipBuffer));
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