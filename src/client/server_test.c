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

#define BUFFER 1024
#define MAX_CLIENTS 100
////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to convert an integer to a string
char* util_int_to_str(int value) {
    char* str = (char*)malloc(15);  // a maximum of 12 digits for an int
    snprintf(str, 15, "%d", value);
    return str;
}

// get current timestamp as string (format: YYYY-mm-dd HH:MM:SS)
void util_get_time(char * timeString){
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timeString, 20, "%Y-%m-%d %H:%M:%S", timeInfo);
}

// get a substring from a binary string
void util_get_substring(const char *string, char *buffer, int pos, int len)
{
    char * temp = (char *) malloc((len + 1)*sizeof(char));
    strcpy(temp, string + pos);
    temp[len] = '\0';  // Null-terminate the string
    strcpy(buffer, temp);
}
///////////////////////////////////////

typedef struct Parameters{
    char Param1[1024];
    char Param2[1024];
    char Param3[1024];
} Parameters;

uint32_t getProtocolOpcode (const char *message);
uint32_t getProtocolFunctionCode (const char *message);
void getProtocolPayload (const char *message, char * buffer, int size);
void getProtocolParameters(const char *payload, Parameters *parameters);
int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer);
void printCode(const char * binaryStr, size_t size);


uint32_t getProtocolOpcode (const char *message){
    // Extract the integer from the first 4 bytes
    uint32_t opCode;
    memcpy(&opCode, message, sizeof(opCode));
    return opCode;
}

void printCode(const char * binaryStr, size_t size){
    
    for (size_t i = 0; i < size; i++) {
        printf("\\x%02X", (unsigned char)binaryStr[i]);
    }
    printf("\n");
}

uint32_t getProtocolFunctionCode (const char *message){
    // Extract the integer from the next 4 bytes
    uint32_t func;
    memcpy(&func, message + sizeof(uint32_t), sizeof(func));
    return func;
}

void getProtocolPayload (const char *message, char * buffer, int size){
    // Extract the payload from the entire of the message
    memcpy(buffer, message + 2*sizeof(uint32_t), size);
}

void getProtocolParameters(const char *payload, Parameters *parameters) {
    // Extract parameters from the payload
    uint32_t paramLen1, paramLen2, paramLen3;
    memcpy(&paramLen1, payload, sizeof(paramLen1));
    memcpy(&paramLen2, payload + sizeof(uint32_t) + paramLen1, sizeof(paramLen2));
    memcpy(&paramLen3, payload + 2 * sizeof(uint32_t) + paramLen1 + paramLen2, sizeof(paramLen3));

    // printf("paramlen1: %d\n", paramLen1);
    // printf("paramlen2: %d\n", paramLen2);
    // printf("paramlen3: %d\n", paramLen3);

    util_get_substring(payload, parameters->Param1, 4, paramLen1);
    util_get_substring(payload, parameters->Param2, 8 + paramLen1, paramLen2);
    util_get_substring(payload, parameters->Param3, 12 + paramLen1 + paramLen2, paramLen3);
}

int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer){
    int len1 = strlen(parameters.Param1);
    int len2 = strlen(parameters.Param2);
    int len3 = strlen(parameters.Param3);
    printf("len1: %d\n", len1);
    printf("len2: %d\n", len2);
    printf("len3: %d\n", len3);

    printf("Param1: %s\n", parameters.Param1);
    printf("Param2: %s\n", parameters.Param2);
    printf("Param3: %s\n", parameters.Param3);

    int bufferSize = 20 + len1 + len2 + len3;

    memcpy(buffer, &op, sizeof(op));
    memcpy(buffer+4, &func, sizeof(func));
    memcpy(buffer+8, (uint32_t*)&len1, sizeof(uint32_t));
    memcpy(buffer+12, parameters.Param1, len1*sizeof(uint32_t));
    memcpy(buffer+12+len1, (uint32_t*)&len2, sizeof(uint32_t));
    memcpy(buffer+16+len1, parameters.Param2, len2*sizeof(uint32_t));
    memcpy(buffer+16+len1+len2, (uint32_t*)&len3, sizeof(uint32_t));
    memcpy(buffer+20+len1+len2, parameters.Param3, len3*sizeof(uint32_t));
    buffer[bufferSize] = '\0';
    printf("buffer generate message: ");
    printCode(buffer, bufferSize);
    return bufferSize;
}
///////////////////////////////////////////////////////////////////////////////////


// // Structure to pass arguments to the thread
// struct ThreadArgs {
//     int clientSocket;
// };

// Structure to represent a message
struct Message {
    char buffer[BUFFER];
    int clientSocket;
    struct Message* next;
};

// Structure to pass arguments to the thread
struct ThreadArgs {
    int clientSocket;
    pthread_cond_t* condVar;
    pthread_mutex_t* mutex;
    struct Message** messageQueue;  // Change the type to struct Message**
};


int readMessage(const char * binaryString, int size) {

    printf("Binary String Read: \n");
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
    if(strcmp(buffer, "PING"))
    printf("Received message from client %d: %s\n", clientSocket, buffer);

    return bytesRead;
}

ssize_t sendMessage(int clientSocket, const void *message, size_t len) {
    size_t totalSent = 0;

    while (totalSent < len) {
        usleep(2000);
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
        // Dequeue a message
        pthread_mutex_lock(threadArgs->mutex);
        while (*threadArgs->messageQueue == NULL) {
            pthread_cond_wait(threadArgs->condVar, threadArgs->mutex);
        }

        struct Message* currentMessage = *threadArgs->messageQueue;
        *threadArgs->messageQueue = currentMessage->next;
        pthread_mutex_unlock(threadArgs->mutex);

        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = receiveMessage(clientSocket, buffer);

        if (bytesReceived <= 0) {
            free(currentMessage);
            break; // Connection closed or error
        }

        // Process the received message
        if (strcmp(buffer, "PING")){
            printf("Processing message from client %d: %s\n", clientSocket, buffer);
            printCode(buffer, bytesReceived);
        }

        usleep(3000);
        
        // Echo the message back to the client
        int sent = sendMessage(clientSocket, buffer, bytesReceived);
        printf("sent: ");
        printCode(buffer, sent);
        free(currentMessage);
    }

    close(clientSocket);
    free(threadArgs); // Free the memory allocated for thread arguments
    pthread_exit(NULL);
}

void enqueueMessage(struct Message** queue, const char* buffer, int clientSocket) {
    struct Message* newMessage = (struct Message*)malloc(sizeof(struct Message));
    if (newMessage == NULL) {
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    strncpy(newMessage->buffer, buffer, BUFFER);
    newMessage->clientSocket = clientSocket;
    newMessage->next = NULL;

    if (*queue == NULL) {
        *queue = newMessage;
    } else {
        struct Message* current = *queue;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newMessage;
    }
}

void runServer(int serverSocket) {
    int clientSockets[MAX_CLIENTS];
    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // Initialize client socket array
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clientSockets[i] = -1;
    }

    // Initialize synchronization objects
    pthread_cond_t condVar = PTHREAD_COND_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    // Initialize message queue
    struct Message* messageQueue = NULL;

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
        threadArgs->condVar = &condVar;
        threadArgs->mutex = &mutex;
        threadArgs->messageQueue = &messageQueue;

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
