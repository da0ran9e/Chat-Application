#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Parameters{
    char Param1[1024];
    char Param2[1024];
    char Param3[1024];
} Parameters;

void substring(const char *string, char *buffer, int pos, int len);
uint32_t getProtocolOpcode (const char *message);
uint32_t getProtocolFunctionCode (const char *message);
void getProtocolPayload (const char *message, char * buffer, int size);
Parameters getProtocolParameters(const char *payload, Parameters parameters);
void undoTranslation(uint32_t intValue, char charValue, char* undoBuffer, size_t bufferSize);
int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer);
int Read(const char * binaryString, int size);

#define BUFFER_SIZE 1024

int initializeClient(const char *address, int port) {
    int clientSocket;
    struct sockaddr_in serverAddr; /* server's address information */

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(address);

    // Request to connect server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr)) < 0) {
        printf("Cannot connect to server!\n");
        close(clientSocket);
        exit(0);
    }

    return clientSocket;
}

ssize_t send_message(int clientSocket, char *message, int len) {
    ssize_t bytesSent = send(clientSocket, message, len, 0);
    if (bytesSent <= 0) {
        perror("Error: ");
        return -1;
    }

    return bytesSent;
}

ssize_t receive_message(int clientSocket, char *buffer) {
    ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesReceived <= 0) {
        perror("NaN received: ");
        return -1;
    }

    buffer[bytesReceived] = '\0'; 
    return bytesReceived;
}

void run_client(const char *address, int port) {
    int clientSocket;
    char message[BUFFER_SIZE];

    clientSocket = initializeClient(address, port);

    while (1) {
        printf("Enter a message (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            // message[len - 1] = '\0';
            // send_message(clientSocket, message);
            Parameters parms;
            strcpy(parms.Param1, "Hello. ");
            strcpy(parms.Param2, "This is message from");
            strcpy(parms.Param3, " Client");

            int siz = 20 + strlen(parms.Param1) + strlen(parms.Param2) + strlen(parms.Param3);
            char buff[siz];
            int bufsiz = generateMessage (1, 4, parms, buff);

            send_message(clientSocket, buff, siz);
        }

        // Exit the loop if the user entered "exit"
        if (strcmp(message, "exit") == 0) {
            break;
        }

        char buffer[BUFFER_SIZE];
        receive_message(clientSocket, buffer);

        printf("Server response: %s\n", buffer);
    }

    close(clientSocket);
}

/////////////////////////////////////////////////////////////////////////////////////

void substring(const char *string, char *buffer, int pos, int len)
{
    // printf("Binary String : \n");
    for (size_t i = 0; i < len; i++) {
        printf("\\x%02X", (unsigned char)string[i]+pos);
    }
    printf("\n");
    char * temp = (char *) malloc((len + 1)*sizeof(char));
    strcpy(temp, string + pos);
    temp[len] = '\0';  // Null-terminate the string
    strcpy(buffer, temp);
}

uint32_t getProtocolOpcode (const char *message){
    // Extract the integer from the first 4 bytes
    uint32_t opCode;
    memcpy(&opCode, message, sizeof(opCode));
    return opCode;
}

uint32_t getProtocolFunctionCode (const char *message){
    // Extract the integer from the next 4 bytes
    uint32_t func;
    memcpy(&func, message + sizeof(uint32_t), sizeof(func));
    return func;
}

void getProtocolPayload (const char *message, char * buffer, int size){
    // Extract the payload from the entire of the message
    memcpy(buffer, message + 2*sizeof(uint32_t), size);
}

Parameters getProtocolParameters(const char *payload, Parameters parameters){
    // Extract parameters from the payload 
    uint32_t paramLen1;
    uint32_t paramLen2;
    uint32_t paramLen3;
    memcpy(&paramLen1, payload, sizeof(paramLen1));
    memcpy(&paramLen2, payload + sizeof(uint32_t) + paramLen1, sizeof(paramLen2));
    memcpy(&paramLen3, payload + 2*sizeof(uint32_t) + paramLen1 + paramLen2, sizeof(paramLen3));
    // printf("ParamLen1: %d\n", paramLen1);
    // printf("ParamLen2: %d\n", paramLen2);
    // printf("ParamLen3: %d\n", paramLen3);

    substring(payload, parameters.Param1, 4, paramLen1);
    substring(payload, parameters.Param2, 8+paramLen1, paramLen2);
    substring(payload, parameters.Param3, 12+paramLen1+paramLen2, paramLen3);
    // printf("Param1: %s\n", parameters.Param1);
    // printf("Param2: %s\n", parameters.Param2);
    // printf("Param3: %s\n", parameters.Param3);
    return parameters;
}

void undoTranslation(uint32_t intValue, char charValue, char* undoBuffer, size_t bufferSize) {
    // Copy the integer into the buffer
    memcpy(undoBuffer, &intValue, sizeof(intValue));

    // Copy the character into the buffer after the integer
    memcpy(undoBuffer + sizeof(intValue), &charValue, sizeof(charValue));
}

int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer){
    int len1 = strlen(parameters.Param1);
    int len2 = strlen(parameters.Param2);
    int len3 = strlen(parameters.Param3);
    printf("ParamLen1: %d\n", len1);
    printf("len2: %d\n", len2);
    printf("len3: %d\n", len3);

    int bufferSize = 20 + len1 + len2 + len3;

    memcpy(buffer, &op, sizeof(op));
    memcpy(buffer+4, &func, sizeof(func));
    memcpy(buffer+8, (uint32_t*)&len1, sizeof(uint32_t));
    memcpy(buffer+12, parameters.Param1, len1*sizeof(uint32_t));
    memcpy(buffer+12+len1, (uint32_t*)&len2, sizeof(uint32_t));
    memcpy(buffer+16+len1, parameters.Param2, len2*sizeof(uint32_t));
    memcpy(buffer+16+len1+len2, (uint32_t*)&len3, sizeof(uint32_t));
    memcpy(buffer+20+len1+len2, parameters.Param3, len3*sizeof(uint32_t));
    buffer[bufferSize-1] = '\0';

    return bufferSize;
}

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        printf("Usage: %s IPAddress PortNumber\n", argv[0]);
        exit(1);
    }

    run_client(argv[1], atoi(argv[2]));

    return 0;
}
