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
#include <ifaddrs.h>

#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100

int initializeClient(const char *address, int port);
ssize_t send_message(int clientSocket, char *message, int len);
ssize_t receive_message(int clientSocket, char *buffer);

#endif