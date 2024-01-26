
/* Get the system IP address */
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

void getNetworkIPAddress() {
    struct ifaddrs* ifaddr, *ifa;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return;
    }

    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        if (ifa->ifa_addr->sa_family == AF_INET) {
            char ipstr[INET_ADDRSTRLEN];
            struct sockaddr_in* addr = (struct sockaddr_in*)ifa->ifa_addr;
            inet_ntop(AF_INET, &(addr->sin_addr), ipstr, INET_ADDRSTRLEN);
            printf("Network Interface: %s\t Network IP Address: %s\n", ifa->ifa_name, ipstr);
        }
    }

    freeifaddrs(ifaddr);
}




/* Setup Server */

int main() {
    getNetworkIPAddress();
    return 0;
}