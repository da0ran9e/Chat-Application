#include "../../include/server/connection.h"

ssize_t receiveMessage(int clientSocket, char *buffer) {
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER - 1, 0);
    if (bytesRead <= 0) {
        perror("Connection lost from: ");
        return -1;
    }
    buffer[bytesRead] = '\0'; // end message
    printf("Received: ");
    printCode(buffer, bytesRead);

    return bytesRead;
}

ssize_t sendMessage(int clientSocket, const void *message, size_t len) {
    printf("Send: ");
    printCode(message, len);
    size_t totalSent = 0;
    while (totalSent < len) {
        ssize_t sent = send(clientSocket, message + totalSent, len - totalSent, 0);

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

void initializeServer(int *serverSocket, int port) {
    struct sockaddr_in serverAddr;

    if ((*serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

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

    printf("Server is listening on port %d...\n", port);
}
