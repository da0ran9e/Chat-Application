#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


#define MAX_LOG_SIZE 1024

void write_log(char *log){
    FILE *logFile = fopen("../log.json", "a");
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

void get_log_time(char * time){
    time_t currentTime = time(NULL);
    struct tm* timeInfo = localtime(&currentTime);
    strftime(time, sizeof(time), "%Y-%m-%d %H:%M:%S", timeInfo);
}

void get_username(char *ip, int id, char *name){
    
}

void serverLog(enum ServerStatus status, int port) {
    char time[20];
    get_log_time(time);

    // Create the log message
    char logMessage[MAX_LOG_SIZE];
    snprintf(logMessage, MAX_LOG_SIZE, "\t{\"%s\": \"[SERVER] %s Port: %d\"}\n}", time, status, port);
    
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

