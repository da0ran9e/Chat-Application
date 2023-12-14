#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER 1024
#define MAX_CLIENTS 100

// Structure to pass arguments to the thread
struct ThreadArgs {
    int clientSocket;
};

// Function to handle I/O asynchronously in a thread
void *handleClient(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = receiveMessage(clientSocket, buffer);

        if (bytesReceived <= 0) {
            break; // Connection closed or error
        }

        // Process the received message
        printf("Processing message from client %d: %s\n", clientSocket, buffer);

        // Echo the message back to the client
        sendMessage(clientSocket, buffer, strlen(buffer));
    }

    // Log disconnection
    connectionLog(DISCONNECT, -1, "Client disconnected");
    close(clientSocket);
    free(threadArgs); // Free the memory allocated for thread arguments
    pthread_exit(NULL);
}

void runServer(int serverSocket) {
    int clientSockets[MAX_CLIENTS];
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Initialize client socket array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clientSockets[i] = -1;
    }

    while (1) {
        // Accept the connection
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Add the new client socket to the array
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clientSockets[i] == -1) {
                clientSockets[i] = clientSocket;
                break;
            }
        }

        printf("New connection from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        // Create thread arguments
        struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
        threadArgs->clientSocket = clientSocket;

        // Create a new thread to handle the client
        pthread_t thread;
        if (pthread_create(&thread, NULL, handleClient, (void *)threadArgs) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }

        // Detach the thread to clean up resources automatically
        pthread_detach(thread);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s PortNumber\n", argv[0]);
        exit(1);
    }

    int serverSocket;
    int port = atoi(argv[1]);

    initializeServer(&serverSocket, port);

    runServer(serverSocket);

    close(serverSocket);

    return 0;
}
