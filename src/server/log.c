#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "../../include/server/log.h"


#define MAX_LOG_SIZE 1024

enum serverStat {
    START,
    END,
};

void write_log(char *log){
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

void get_log_time(char * timeStr){
    time_t rawtime;
    struct tm* timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format the time as YYYY-MM-DD hh:mm:ss
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeinfo);

    printf("Current time: %s\n", timeStr);
    }

void get_username(char *ip, int id, char *name){

}

void serverLog(enum ServerStat status, int port) {
    char time[20];
    get_log_time(time);

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
        break;
    }

    // Create the log message
    char logMessage[MAX_LOG_SIZE];
    snprintf(logMessage, MAX_LOG_SIZE, "\t{\"%s\": \"[SERVER] %s Port: %d\"}\n}", time, stat, port);
    
    write_log(logMessage);
}

void clientLog(enum ClientStatus status, int port, char *ipAddress){
    char time[20];
    get_log_time(time);

    int userid;
    char username[50];

    get_username(ipAddress, userid, username);

    // Create the log message
    char logMessage[MAX_LOG_SIZE];
    snprintf(logMessage, MAX_LOG_SIZE, "\t{\"%s\": \"[CLIENT]%s %s as %s address: %s port: %d\"}\n}", time, userid, status, username, ipAddress, port);
    
    write_log(logMessage);
}

