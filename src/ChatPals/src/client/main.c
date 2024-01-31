#include "../../include/client/feature.h"

int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("Usage: %s ServerIPAddress PortNumber\n", argv[0]);
        exit(1);
    }
    char *clientIP = argv[1];
    int clientPort = atoi(argv[2]);

    run_client(clientIP,clientPort);

    void showFeatures();
}