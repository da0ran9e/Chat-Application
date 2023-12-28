#include "../../include/client/feature.h"

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
        handle_receive_message(buffer);
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
        char pingMessage[BUFFER];
        Parameters p;
        int len = generateMessage(0, 0, p, pingMessage);

        send(clientSocket, pingMessage, len, 0);

        sleep(PING_INTERVAL);
        gettimeofday(&endTime, NULL);
        g_rtd = (endTime.tv_sec - startTime.tv_sec) * 1000000L +
                                (endTime.tv_usec - startTime.tv_usec);
    }

    pthread_exit(NULL);
}

void run_client(const char *address, const int port){
    strcpy(g_address, address);
    g_port = port;
    g_socket = initializeClient(g_address, g_port);
    // Create thread arguments
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    threadArgs->clientSocket = g_socket;

    // Create threads for sending and receiving messages, and sending ping messages
    pthread_t sendThreadID, receiveThreadID, pingThreadID;
    if (pthread_create(&sendThreadID, NULL, sendThread, (void *)threadArgs) != 0 ||
        pthread_create(&receiveThreadID, NULL, receiveThread, (void *)threadArgs) != 0 ||
        pthread_create(&pingThreadID, NULL, sendPingMessages, (void *)threadArgs) != 0) {
        perror("Error creating threads");
        close(g_socket);
        free(threadArgs);
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(sendThreadID, NULL);
    pthread_join(receiveThreadID, NULL);
    pthread_join(pingThreadID, NULL);

    close(g_socket);
    free(threadArgs);
}