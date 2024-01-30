#include "../../include/server/connection.h"


// Get local area network ip address
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

// debuger
void printCodes(const char * binaryStr, size_t size){
    for (size_t i = 0; i < size; i++) {
        printf("\\x%02X", (unsigned char)binaryStr[i]);
    }
    printf("\n");
}

// receive message from client
ssize_t receiveMessage(int clientSocket, char *buffer) {
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER - 1, 0);
    if (bytesRead <= 0) {
        perror("Connection lost from: ");
        return -1;
    }
    buffer[bytesRead] = '\0'; // end message
    // printf("Received: ");
    // printCodes(buffer, bytesRead);

    return bytesRead;
}

//send message to client
ssize_t sendMessage(int clientSocket, const void *message, size_t len) {
    printf("About to send: ");
    printCodes(message, len);
    size_t totalSent = 0;
    while (totalSent < len) {
        ssize_t sent = send(clientSocket, message + totalSent, len - totalSent, 0);
        // printf("Sent: ");
        // printCodes(message, sent);
        if (sent == -1) {
            perror("Error in send");
            return -1;
        } else if (sent == 0) {
            fprintf(stderr, "Client %d is not online!\n", clientSocket);
            return 0;
        }
        totalSent += sent;
    }
    return totalSent;
}

// start initially server properties 
void initializeServer(int *serverSocket, int port) {
    struct sockaddr_in serverAddr;

    if ((*serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);

    // Get the local IP address
    char ipBuffer[INET_ADDRSTRLEN];
    getLocalIPAddress(ipBuffer, sizeof(ipBuffer), 0);

    if (inet_pton(AF_INET, ipBuffer, &serverAddr.sin_addr) != 1) {
        perror("Error converting IP address");
        close(*serverSocket);
        exit(EXIT_FAILURE);
    }

    if (bind(*serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        close(*serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(*serverSocket, 5) == -1) {
        perror("Error listening for connections");
        close(*serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server LAN: %s\n", ipBuffer);
    printf("Server is listening on port %d...\n", port);
}
