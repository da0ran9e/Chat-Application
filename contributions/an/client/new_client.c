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

void run_client(const char *address, int port) {
    int clientSocket;
    char message[BUFFER_SIZE];

    clientSocket = initializeClient(address, port);

    while (1) {
        printf("Enter a message (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        // Remove newline character from the input
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
        }

        // Send the message to the server
        if (send_message(clientSocket, message, strlen(message)) == -1) {
            break;
        }

        // Check for the exit condition
        if (strcmp(message, "exit") == 0) {
            printf("Exiting client...\n");
            break;
        }

        // Receive and print the server's response
        char buffer[BUFFER_SIZE];
        ssize_t bytesReceived = receive_message(clientSocket, buffer);
        if (bytesReceived == -1) {
            break;
        }

        printf("Server response: %s\n", buffer);
    }

    // Close the client socket
    close(clientSocket);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s ServerIPAddress PortNumber\n", argv[0]);
        exit(1);
    }

    const char *address = argv[1];
    int port = atoi(argv[2]);

    run_client(address, port);

    return 0;
}
