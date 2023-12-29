#include "../../include/shared/common.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENTS 100

// Structure to pass arguments to the thread
struct ThreadArgs {
    int clientSocket;
};

int readMessage(const char * binaryString, int size) {
    printf("Opcode: %d\n", getProtocolOpcode(binaryString));
    printf("Func: %d\n", getProtocolFunctionCode(binaryString));
    char payload[size];

    getProtocolPayload(binaryString, payload, sizeof(payload));
    Parameters p;
    getProtocolParameters(payload, &p);
    
    printf("len1: %lu\n", strlen(p.Param1));
    printf("Param1: %s\n", p.Param1);
    printf("len2: %lu\n", strlen(p.Param2));
    printf("Param2: %s\n", p.Param2);
    printf("len3: %lu\n", strlen(p.Param3));
    printf("Param3: %s\n", p.Param3);

    

    // Print the binary string

    printf("Binary String : \n");
    for (size_t i = 0; i < size; i++) {
        printf("\\x%02X", (unsigned char)binaryString[i]);
    }
    printf("\n");

    return 0;
}

ssize_t receiveMessage(int clientSocket, char *buffer) {
    ssize_t bytesRead = recv(clientSocket, buffer, BUFFER - 1, 0);
    if (bytesRead <= 0) {
        perror("Connection lost from: ");
        return -1;
    }
    buffer[bytesRead] = '\0'; // end message
    printf("Received message from client %d: %s\n", clientSocket, buffer);

    return bytesRead;
}

ssize_t sendMessage(int clientSocket, const void *message, size_t len) {
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

        int op;
        int func;
        Parameters params;
        char send_buffer[BUFFER];
        readMessage(buffer, sizeof(buffer));
        printf("Type message send to client:\n");
        printf("opcode: ");
        scanf("%d", &op);
        printf("func: ");
        scanf("%d", &func);
        printf("param1: ");
        scanf("%s", params.Param1);
        printf("param2: ");
        scanf("%s", params.Param2);
        printf("param3: ");
        scanf("%s", params.Param3);

        int len = generateMessage(op, func, params, send_buffer);
        // Echo the message back to the client
        sendMessage(clientSocket, send_buffer, len);
    }

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
