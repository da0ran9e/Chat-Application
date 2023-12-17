#include "../../include/server/log.h"

void write_log(const char *log){
    FILE *logFile = fopen("../../log.json", "a");
    if (logFile == NULL) {
        perror("Error opening log file");
        exit(EXIT_FAILURE);
    }

    // Remove the trailing '}' character if it exists
    fseek(logFile, -1, SEEK_END);
    if (ftell(logFile) > 0) {
        ftruncate(fileno(logFile), ftell(logFile));
    }

    fputs(log, logFile);
    fclose(logFile);
}

void get_username(const char *ip, int id){
}

void serverLog(enum ServerStat status, int port) {
    char time[20];
    util_get_time(time);

    char stat[20];

    switch (status)
    {
    case START:
        strcpy(stat, "START");
        break;
    case END:
        strcpy(stat, "END");
        break;
    default:
        strcpy(stat, "UNKNOWN");
        break;
    }

    // Create the log message
    char logMessage[BUFFER];
    snprintf(logMessage, BUFFER, "\t{\"%s\": \"[SERVER] %s Port: %d\"}\n}", time, stat, port);
    
    write_log(logMessage);
}

void connectionLog(enum ServerStat status, int port, const char *ip) {
    char time[20];
    util_get_time(time);

    char stat[20];

    switch (status)
    {
    case START:
        strcpy(stat, "CONNECT");
        break;
    default:
        strcpy(stat, "UNKNOWN");
        break;
    }

    // Create the log message
    char logMessage[BUFFER];
    snprintf(logMessage, BUFFER, "\t{\"%s\": \"[SERVER] %d %s ip: %s\"}\n}", time, port, stat, ip);
    
    write_log(logMessage);
}

/*
void clientLog(enum ClientStat status, int port, char *ipAddress){
    char time[20];
    util_get_time(time);

    int userid;
    char username[50];

    get_username(ipAddress, userid, username);

    // Create the log message
    char logMessage[MAX_LOG_SIZE];
    snprintf(logMessage, MAX_LOG_SIZE, "\t{\"%s\": \"[CLIENT]%s %s as %s address: %s port: %d\"}\n}", time, userid, status, username, ipAddress, port);
    
    write_log(logMessage);
}

*/