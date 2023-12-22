#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX_LOG_SIZE 1024

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