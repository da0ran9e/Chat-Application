#include "../../include/client/feature.h"

void sendMessage(void *args, const char *buffer, int size)
{
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;

    pthread_mutex_lock(&threadArgs->threadMutex);
    send(clientSocket, buffer, size, 0);

    pthread_mutex_unlock(&threadArgs->threadMutex);
    usleep(PING_INTERVAL * 3000);
}

int recvAndProcess(void *args)
{
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER - 1, 0);

    if (bytesReceived <= 0)
    {
        printf("Server disconnected.\n");
    }

    return handle_receive_message(buffer, bytesReceived);
}

// Function to send messages to the server
void *sendThread(void *args)
{
    g_args = args;
    char buffer[BUFFER];

    while (1)
    {
        showFeatures();
    }

    pthread_exit(NULL);
}

// Function to send ping messages to the server
void *sendPingMessages(void *args)
{
    struct timeval startTime, endTime;

    while (1)
    {
    }

    pthread_exit(NULL);
}

void run_client(const char *address, const int port){
    
    g_socket = -1;
    g_port = -1;
    g_rtd = 9999;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        g_user[i][0] = '\0';
        g_rtds[i] = 9999;
        g_friend[i][0] = '\0';
        g_rooms[i].roomId = -1;
    }
    strcpy(g_address, address);
    g_port = port;
    g_socket = initializeClient(g_address, g_port);

    // Create thread arguments
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    threadArgs->clientSocket = g_socket;
    pthread_mutex_init(&threadArgs->threadMutex, NULL); // Initialize the mutex

    // Create threads for sending and receiving messages, and sending ping messages
    pthread_t sendThreadID, pingThreadID;
    if (pthread_create(&sendThreadID, NULL, sendThread, (void *)threadArgs) != 0 ||
        pthread_create(&pingThreadID, NULL, sendPingMessages, (void *)threadArgs) != 0)
    {
        perror("Error creating threads");
        close(g_socket);
        free(threadArgs);
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(sendThreadID, NULL);
    pthread_join(pingThreadID, NULL);

    // Clean up and destroy the mutex
    pthread_mutex_destroy(&threadArgs->threadMutex);
    close(g_socket);
    free(threadArgs);

}