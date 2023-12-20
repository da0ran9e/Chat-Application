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

void serverLog(enum ServerStat status, int port) {
    char time[20];
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(time, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

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
    char logMessage[1024];
    snprintf(logMessage, 1024, "\t{\"%s\": \"[SERVER] %s Port: %d\"}\n}", time, stat, port);
    
    write_log(logMessage);
}

void connectionLog(enum ServerStat status, int port, const char *ip) {
    char time[20];
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(time, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

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
    char logMessage[1024];
    snprintf(logMessage, 1024, "\t{\"%s\": \"[SERVER] %d %s ip: %s\"}\n}", time, port, stat, ip);
    
    write_log(logMessage);
}

void clientLog(enum ClientStat status, const char * username, const char *ipAddress, const int port){
    char time[20];
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(time, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

    // Create the log message
    char logMessage[MAX_LOG_SIZE];
    snprintf(logMessage, MAX_LOG_SIZE, "\t{\"%s\": \"[CLIENT] %s as %s address: %s port: %d\"}\n}", time, status, username, ipAddress, port);
    
    write_log(logMessage);
}

