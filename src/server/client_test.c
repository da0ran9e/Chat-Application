#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#include "../../include/shared/common.h"

#define PING_INTERVAL 5 // Interval in seconds for sending ping messages

// Structure to pass arguments to the thread
struct ThreadArgs {
    int clientSocket;
};

// Function to send messages to the server
void *sendThread(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    while (1) {
        int op;
        int func;
        Parameters params;
        
        printf("Enter Opcode: \n");
        scanf("%d",&op);
        printf("Enter Func: \n");
        scanf("%d",&func);
        printf("Enter param 1: \n");
        scanf("%s", params.Param1);
        printf("Enter param 2: \n");
        scanf("%s", params.Param2);
        printf("Enter param 3: \n");
        scanf("%s", params.Param3);

        int len = generateMessage(op, func, params, buffer);

        // // Get user input and send to the server
        // printf("Enter message to send to the server: ");
        // fgets(buffer, BUFFER, stdin);



        // Send the message to the server
        send(clientSocket, buffer, len, 0);
    }

    pthread_exit(NULL);
}

// Function to receive messages from the server
void *receiveThread(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    while (1) {
        // Receive messages from the server
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER - 1, 0);

        if (bytesReceived <= 0) {
            // Connection closed or error, handle disconnect event
            printf("Server disconnected.\n");
            break;
        }

        // Process the received message
        printf("Received message from server: %s\n", buffer);
    }

    pthread_exit(NULL);
}

// Function to send ping messages to the server
void *sendPingMessages(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    struct timeval startTime, endTime;

    while (1) {
        gettimeofday(&startTime, NULL);

        // Send a ping message to the server
        const char *pingMessage = "PING";
        send(clientSocket, pingMessage, strlen(pingMessage), 0);

        sleep(PING_INTERVAL);

        // Calculate and print round-trip delay time
        gettimeofday(&endTime, NULL);
        long int roundTripTime = (endTime.tv_sec - startTime.tv_sec) * 1000000L +
                                (endTime.tv_usec - startTime.tv_usec);
        printf("Round-trip delay time: %ld microseconds\n", roundTripTime);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s ServerIPAddress PortNumber\n", argv[0]);
        exit(1);
    }

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
    }

    // Create thread arguments
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    threadArgs->clientSocket = clientSocket;

    // Create threads for sending and receiving messages, and sending ping messages
    pthread_t sendThreadID, receiveThreadID, pingThreadID;
    if (pthread_create(&sendThreadID, NULL, sendThread, (void *)threadArgs) != 0 ||
        pthread_create(&receiveThreadID, NULL, receiveThread, (void *)threadArgs) != 0 ||
        pthread_create(&pingThreadID, NULL, sendPingMessages, (void *)threadArgs) != 0) {
        perror("Error creating threads");
        close(clientSocket);
        free(threadArgs);
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(sendThreadID, NULL);
    pthread_join(receiveThreadID, NULL);
    pthread_join(pingThreadID, NULL);

    close(clientSocket);
    free(threadArgs);

    return 0;
}
