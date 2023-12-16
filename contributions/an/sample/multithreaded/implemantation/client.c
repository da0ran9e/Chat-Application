#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int initializeClient(const char *address, int port) {
    int clientSocket;
    struct sockaddr_in serverAddr; /* server's address information */

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(address);

    // Request to connect server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) < 0) {
        printf("Cannot connect to server!\n");
        close(clientSocket);
        exit(0);
    }

    return clientSocket;
}

ssize_t send_message(int clientSocket, char *message) {
    char testStr[] = "\x01\x00\x00\x00\x0f\x00\x00\x00\x0a\x00\x00\x00helloworld\x06\x00\x00\x00this's\x08\x00\x00\x00protocol";

    ssize_t bytesSent = send(clientSocket, testStr, 31, 0);
    if (bytesSent <= 0) {
        perror("Error: ");
        return -1;
    }

    return bytesSent;
}

ssize_t receive_message(int clientSocket, char *buffer) {
    ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesReceived <= 0) {
        perror("NaN received: ");
        return -1;
    }

    buffer[bytesReceived] = '\0'; 
    return bytesReceived;
}

void run_client(const char *address, int port) {
    int clientSocket;
    char message[BUFFER_SIZE];

    clientSocket = initializeClient(address, port);

    while (1) {
        printf("Enter a message (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
            send_message(clientSocket, message);
        }

        // Exit the loop if the user entered "exit"
        if (strcmp(message, "exit") == 0) {
            break;
        }

        char buffer[BUFFER_SIZE];
        receive_message(clientSocket, buffer);

        printf("Server response: %s\n", buffer);
    }

    close(clientSocket);
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        printf("Usage: %s IPAddress PortNumber\n", argv[0]);
        exit(1);
    }

    run_client(argv[1], atoi(argv[2]));

    return 0;
}
