#include "../../include/client/feature.h"

int main(){
    // Get the local IP address
    char ipBuffer[INET_ADDRSTRLEN];
    printf("Server LAN address: \n");
    scanf("%s", ipBuffer);

    int clientPort;
    printf("Connect to which port?( Enter a number ) ");
    scanf("%d", &clientPort);

    run_client(ipBuffer,clientPort);

    void showFeatures();
}