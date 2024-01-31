#ifndef LOG_H
#define LOG_H

#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_LOG_SIZE 1024


enum RequestEvent {
    REQ_PING            = 00,
    REQ_LOGIN           = 10,
    REQ_REGISTER        = 20,
    REQ_CHANGE_PASS     = 30,
    REQ_FRIEND_LIST     = 01,
    REQ_FRIEND_REQUEST  = 11,
    REQ_RESPONSE_REQUEST= 21,
    REQ_ROOM_LIST       = 02,
    REQ_ROOM_MEMBER     = 12,
    REQ_CREATE_ROOM     = 22,
    REQ_ADD_MEMBER      = 32,
    REQ_REMOVE_MEMBER   = 42,
    REQ_CONVERSATION    = 03,
    REQ_MESSAGE         = 13,
};

enum ServerStat {
    CONNECT,
    START,
    END,
};

enum ClientStat {
    LOGIN,
    LOGOUT,
    REGISTER,
};

void write_log(const char *log);
void get_username(const char *ip, int id);
void serverLog(enum ServerStat status, int port);
void connectionLog(enum ServerStat status, int port, const char *ip);
void clientLog(enum ClientStat status, const char * username, const char *ipAddress,const int port);
void eventLog(enum RequestEvent status, int port);

#endif