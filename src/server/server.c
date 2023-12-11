#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER 1024
#include "../../include/server.h"
#include "../../include/shared/constants.h"
// #include "../../include/server/log.h"

#define MAX_CLIENTS 10

ssize_t receiveMessage (int clientSocket, char *buffer) {
    ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        perror("Connection lost from: ");
        return -1;
    }
    buffer[bytesRead] = '\0'; // end message
    printf("Received message from client %d: %s\n", clientSocket, buffer);

    return bytesRead;
}

ssize_t sendMessage (int clientSocket, const void *message, size_t len) {
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

void initializeServer(int serverSocket) {
    struct sockaddr_in serverAddr;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverSocket);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", serverSocket);
}

void runServer(int serverSocket){
    int clientSockets[MAX_CLIENTS];
    fd_set readfds, allset;
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[BUFFER];

    // Initialize client socket array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clientSockets[i] = -1;
    }

    FD_ZERO(&allset);
    FD_SET(serverSocket, &allset);

    while (1) {
        readfds = allset;

        // Wait for activity on any socket
        if (select(FD_SETSIZE, &readfds, NULL, NULL, NULL) == -1) {
            perror("Select failed");
            exit(EXIT_FAILURE);
        }

        // Check for incoming connection on the server socket
        if (FD_ISSET(serverSocket, &readfds)) {
            // Accept the connection
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);
            if (clientSocket == -1) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            // Add the new client socket to the array
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clientSockets[i] == -1) {
                    clientSockets[i] = clientSocket;
                    FD_SET(clientSocket, &allset);
                    break;
                }
            }

            printf("New connection from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        }

        // Check for data from existing clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int clientSocket = clientSockets[i];

            if (clientSocket != -1 && FD_ISSET(clientSocket, &readfds)) {
                memset(buffer, 0, sizeof(buffer));
                ssize_t bytesReceived = receiveMessage(clientSocket, buffer);

                printf ("%s\n", buffer);
            }
        }
    }

    // Close all client sockets
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clientSockets[i] != -1) {
            close(clientSockets[i]);
        }
    }
}

int main(int argc, char *argv[]){
	if (argc != 2) {
		printf("Usage: %s PortNumber\n", argv[0]);
		exit(1);
	}
    
    int serverSocket = atoi(argv[1]);
    int clientSocket = 0;

    initializeServer(serverSocket);

    runServer(serverSocket);

    // serverLog(RUNNING, serverSocket);

    close(serverSocket);

    return 0;
}
