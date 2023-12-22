#ifndef FEATURE_H
#define FEATURE_H

#include "../shared/common.h"
#include "connection.h"

#define MAX_CLIENTS 100

typedef struct RoomMember{
    int roomId;
    char memName[50];
    memName[0] = '\0';
} RoomMember;

int g_socket = -1;
char g_address[15];
int g_port;
int g_rtd;
char g_user[MAX_CLIENTS][50];
int g_rtds[MAX_CLIENTS];
char g_friend[MAX_CLIENTS][50];
Room g_rooms[MAX_CLIENTS];
Message g_message[BUFFER];
RoomMember g_room_member[10000];

int deserializeMessage(const char * message, Parameters param);


#endif