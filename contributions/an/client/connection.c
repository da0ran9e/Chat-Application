#include "../../include/client/connection.h"


int initializeClient(const char *address, int port) {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) <= 0) {
        perror("Invalid server address");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket);
        exit(EXIT_FAILURE);

    return clientSocket;
}

ssize_t send_message(int clientSocket, char *message, int len) {
    ssize_t bytesSent = send(clientSocket, message, len, 0);
    if (bytesSent <= 0) {
        perror("Error sending message: ");
        return -1;
    }

    return bytesSent;
}

ssize_t receive_message(int clientSocket, char *buffer) {
    ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesReceived <= 0) {
        perror("Error receiving message: ");
        return -1;
    }

    buffer[bytesReceived] = '\0';
    return bytesReceived;
}
