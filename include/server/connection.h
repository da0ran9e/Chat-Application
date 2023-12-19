#ifndef CONNECTION_H
#define CONNECTION_H


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

ssize_t receiveMessage(int clientSocket, char *buffer);
ssize_t sendMessage(int clientSocket, const void *message, size_t len);
void initializeServer(int *serverSocket, int port);
void *handleClient(void *args);
void runServer(int serverSocket);
int readMessage(const char * binaryString, int size);

#endif