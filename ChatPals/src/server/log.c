#include "../../include/server/log.h"

// Function to open the json file and start writing
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
    char timeStr[20];
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

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
    snprintf(logMessage, 1024, "\t{\"%s\": \"[SERVER] %s Port: %d\"}\n}", timeStr, stat, port);
    
    write_log(logMessage);
}

void connectionLog(enum ServerStat status, int port, const char *ip) {
    char timeStr[20];
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

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
    snprintf(logMessage, 1024, "\t{\"%s\": \"[CONNECT] %d %s ip: %s\"}\n}", timeStr, port, stat, ip);
    
    write_log(logMessage);
}

void eventLog(enum RequestEvent status, int port) {
    char timeStr[20];
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

    char stat[20];

    switch (status)
    {
    case 00:
        strcpy(stat,"PING            ");
        break;
    case 10:
        strcpy(stat,"LOGIN           ");
        break;
    case 20:
        strcpy(stat,"REGISTER        ");
        break;
    case 30:
        strcpy(stat,"CHANGE_PASS     ");
        break;
    case 01:
        strcpy(stat,"FRIEND_LIST     ");
        break;
    case 11:
        strcpy(stat,"FRIEND_REQUEST  ");
        break;
    case 21:
        strcpy(stat,"RESPONSE_REQUEST");
        break;
    case 02:
        strcpy(stat,"ROOM_LIST       ");
        break;
    case 12:
        strcpy(stat,"ROOM_MEMBER     ");
        break;
    case 22:
        strcpy(stat,"CREATE_ROOM     ");
        break;
    case 32:
        strcpy(stat,"ADD_MEMBER      ");
        break;
    case 42:
        strcpy(stat,"REMOVE_MEMBER   ");
        break;
    case 03:
        strcpy(stat,"CONVERSATION    ");
        break;
    case 13:
        strcpy(stat,"MESSAGE         ");
        break;
    default:
        strcpy(stat,"UNKNOWN          ");
        break;
    }

    // Create the log message
    char logMessage[1024];
    snprintf(logMessage, 1024, "\t{\"%s\": \"[EVENT] %s Port: %d\"}\n}", timeStr, stat, port);
    
    write_log(logMessage);
}

void clientLog(enum ClientStat status, const char * username, const char *ipAddress, const int port){
    char timeStr[20];
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timeStr, 20, "%Y-%m-%d %H:%M:%S", timeInfo);

    char stat[20];

    switch (status)
    {
    case LOGIN:
        strcpy(stat,"LOGIN");
        break;
    case LOGOUT:
        strcpy(stat,"LOGOUT");
        break;
    case REGISTER:
        strcpy(stat,"REGISTER");
        break;
    default:
        strcpy(stat,"UNKNOWN");
        break;
    }

    // Create the log message
    char logMessage[MAX_LOG_SIZE];
    snprintf(logMessage, MAX_LOG_SIZE, "\t{\"%s\": \"[CLIENT] %s as %s address: %s port: %d\"}\n}", timeStr, stat, username, ipAddress, port);
    
    write_log(logMessage);
}

