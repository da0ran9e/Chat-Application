#ifndef FEATURE_H
#define FEATURE_H

#include "../shared/common.h"
#include "connection.h"

#define MAX_CLIENTS 100

int g_socket = -1;
char g_address[15];
int g_port;
int g_rtd;
Room g_rooms[MAX_CLIENTS];
Message g_message[BUFFER];

int deserializeMessage(const char * message, Parameters param);
void storeRoom

#endif