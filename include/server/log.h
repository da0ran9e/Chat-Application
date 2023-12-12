#ifndef LOG_H
#define LOG_H

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

void write_log(char *log);
void get_log_time(char * timeString);
void get_username(char *ip, int id, char *name);
void serverLog(enum ServerStat status, int port);
void connectionLog(enum ServerStat status, int port, char *ip);
//void clientLog(enum ClientStat status, int port, char *ipAddress);

#endif