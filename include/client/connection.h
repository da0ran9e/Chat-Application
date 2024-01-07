#ifndef CONNECTION_H
#define CONNECTION_H

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

#include "../shared/common.h"

#define BUFFER_SIZE 1024
#define PING_INTERVAL 500 // Interval in seconds for sending ping messages

// Structure to pass arguments to the thread
struct ThreadArgs
{
    int clientSocket;
    pthread_mutex_t threadMutex;
};

typedef struct RoomMember{
    int roomId;
    char memName[50];
    //memName[0] = '\0';
} RoomMember;

int g_socket;
char g_address[15];
int g_port;
int g_rtd;
char g_user[MAX_CLIENTS][50];
int g_rtds[MAX_CLIENTS];
char g_friend[MAX_CLIENTS][50];
Room g_rooms[MAX_CLIENTS];
Message g_message[BUFFER];
RoomMember g_room_member[10000];

int initializeClient(const char *address, int port);
ssize_t send_message(int clientSocket, char *message, int len);
ssize_t receive_message(int clientSocket, char *buffer);

#endif