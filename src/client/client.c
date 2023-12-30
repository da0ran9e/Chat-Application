#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/time.h>

#include "../../include/shared/common.h"
#define MAX_CLIENTS 100

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

typedef struct RoomMember{
    int roomId;
    char memName[50];
    //memName[0] = '\0';
} RoomMember;

int g_socket;
char g_address[15];
int g_port;
int g_rtd;
char g_user[MAX_CLIENTS][50];
int g_rtds[MAX_CLIENTS];
char g_friend[MAX_CLIENTS][50];
Room g_rooms[MAX_CLIENTS];
Message g_message[BUFFER];
RoomMember g_room_member[MAX_CLIENTS*MAX_CLIENTS];

#define BUFFER_SIZE 1024
#define PING_INTERVAL 5 // Interval in seconds for sending ping messages

int initializeClient(const char *address, int port) {
    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, address, &serverAddr.sin_addr) <= 0) {
        perror("Invalid server address");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error connecting to the server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }
    return clientSocket;
}

ssize_t send_message(int clientSocket, char *message, int len) {
    ssize_t bytesSent = send(clientSocket, message, len, 0);
    if (bytesSent <= 0) {
        perror("Error sending message: ");
        return -1;
    }

    return bytesSent;
}

ssize_t receive_message(int clientSocket, char *buffer) {
    ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
    if (bytesReceived <= 0) {
        perror("Error receiving message: ");
        return -1;
    }

    buffer[bytesReceived] = '\0';
    return bytesReceived;
}



// Structure to pass arguments to the thread
struct ThreadArgs {
    int clientSocket;
};

// Function to send messages to the server
void *sendThread(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    while (1) {
        int op;
        int func;
        Parameters params;
        
        printf("Enter Opcode: \n");
        scanf("%d",&op);
        printf("Enter Func: \n");
        scanf("%d",&func);
        printf("Enter param 1: \n");
        scanf("%s", params.Param1);
        printf("Enter param 2: \n");
        scanf("%s", params.Param2);
        printf("Enter param 3: \n");
        scanf("%s", params.Param3);

        int len = generateMessage(op, func, params, buffer);

        // // Get user input and send to the server
        // printf("Enter message to send to the server: ");
        // fgets(buffer, BUFFER, stdin);



        // Send the message to the server
        send(clientSocket, buffer, len, 0);
    }

    pthread_exit(NULL);
}

// Function to receive messages from the server
void *receiveThread(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    while (1) {
        // Receive messages from the server
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER - 1, 0);

        if (bytesReceived <= 0) {
            // Connection closed or error, handle disconnect event
            printf("Server disconnected.\n");
            break;
        }

        // Process the received message
        printf("Received message from server: \n");
        printCode(buffer, bytesReceived);
        //handle_receive_message(buffer);
    }

    pthread_exit(NULL);
}

// Function to send ping messages to the server
void *sendPingMessages(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    struct timeval startTime, endTime;

    while (1) {
        gettimeofday(&startTime, NULL);
        // Send a ping message to the server
        char pingMessage[BUFFER];
        Parameters p;
        int len = generateMessage(0, 0, p, pingMessage);

        send(clientSocket, pingMessage, len, 0);

        sleep(PING_INTERVAL);
        gettimeofday(&endTime, NULL);
        g_rtd = (endTime.tv_sec - startTime.tv_sec) * 1000000L +
                                (endTime.tv_usec - startTime.tv_usec);
    }

    pthread_exit(NULL);
}

void run_client(const char *address, const int port){
    g_socket = -1;
    g_port = -1;
    g_rtd = 9999;
    for (int i=0; i<MAX_CLIENTS; i++){
        g_user[i][0] = '\0';
        g_rtds[i] = 9999;
        g_friend[i][0] = '\0';
        g_rooms[i].roomId = -1;
    }
    strcpy(g_address, address);
    g_port = port;
    g_socket = initializeClient(g_address, g_port);
    // Create thread arguments
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    threadArgs->clientSocket = g_socket;

    // Create threads for sending and receiving messages, and sending ping messages
    pthread_t sendThreadID, receiveThreadID, pingThreadID;
    if (pthread_create(&sendThreadID, NULL, sendThread, (void *)threadArgs) != 0 ||
        pthread_create(&receiveThreadID, NULL, receiveThread, (void *)threadArgs) != 0 ||
        pthread_create(&pingThreadID, NULL, sendPingMessages, (void *)threadArgs) != 0) {
        perror("Error creating threads");
        close(g_socket);
        free(threadArgs);
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(sendThreadID, NULL);
    pthread_join(receiveThreadID, NULL);
    pthread_join(pingThreadID, NULL);

    close(g_socket);
    free(threadArgs);
}

int handle_send_message(enum RequestEvent request, const Parameters params, char * message){
    int op = request%10;
    int func = (request%100)/10;
    int len = generateMessage(op, func, params, message);
    printf("handle_send_message: ");
    printCode(message,len);
    send_message(g_socket, message, len);
    return len;
}
/*
int handle_receive_message(const char * messsge){
    int op = getProtocolOpcode(messsge);
    int func = getProtocolFunctionCode(messsge);
    int code = 10*func + op;
    int status;
    Parameters params;
    char payload[BUFFER-8];
    int plSize=0;
    getProtocolPayload(messsge, payload, plSize);
    getProtocolParameters(payload, &params);
    
    //handle
    switch (code)
    {
    case 00:
        in_online_list(params.Param1, atoi(params.Param2));
        status = 200;
        break;
    case 10:
        if (!strcmp(params.Param1, "error")){
            status = 310;
        }else{
            status = 210;
            in_login_done(params.Param1);
        }
        break;
    case 20:
        if (!strcmp(params.Param1, "error")){
            status = 320;
        }else{
            status = 220;
        }
        break;
    case 30:
        if (!strcmp(params.Param1, "error")){
            status = 330;
        }else{
            status = 230;
        }
        break;
    case 01:
        if (!strcmp(params.Param1, "error")){
            status = 301;
        }else{
            status = 201;
            in_friend_list(params.Param1);
        }
        break;
    case 11:
        if (!strcmp(params.Param1, "error")){
            status = 311;
        }else{
            status = 211;
        }
        break;
    case 21:
        if (!strcmp(params.Param1, "error")){
            status = 321;
        }else if (!strcmp(params.Param1, "accept")){
            status = 221;
        }
        else {
            status = 121;
        }
        break;
    case 02:
        if (!strcmp(params.Param1, "error")){
            status = 302;
        }else{
            status = 202;
            in_room_list(atoi(params.Param1), params.Param2);
        }
        break;
    case 12:
        if (!strcmp(params.Param1, "error")){
            status = 312;
        }else{
            status = 212;
            in_friend_list(params.Param1);
        }
        break;
    case 22:
        if (!strcmp(params.Param1, "error")){
            status = 322;
        }else{
            status = 222;
            in_room_list(atoi(params.Param1),"\0");
        }
        break;
    case 32:
        if (!strcmp(params.Param1, "error")){
            status = 332;
        }else{
            status = 232;
        }
        break;
    case 42:
        if (!strcmp(params.Param1, "error")){
            status = 342;
        }else{
            status = 242;
        }
        break;
    case 03:
        if (!strcmp(params.Param1, "error")){
            status = 303;
        }else{
            status = 203;
            in_conversation(atoi(params.Param1), params.Param2, params.Param3);
        }
        break;
    case 13:
        if (!strcmp(params.Param1, "error")){
            status = 313;
        }else{
            status = 213;
            in_message(atoi(params.Param1), params.Param2, params.Param3);
        }
        break;
    default:
        break;
    }
    return status;
}
*/
void in_online_list(const char * username, const int rtd){
    for(int i=0; i<MAX_CLIENTS; i++){
        if (g_user[i][0]!='\0') {
            strcpy(g_user[i], username);
            g_rtds[i] = rtd;
        }
    }
}

void in_friend_list(const char * username){
    for(int i=0; i<MAX_CLIENTS; i++){
        if (g_friend[i][0]!='\0') {
            strcpy(g_friend[i], username);
        }
    }
}

void in_room_list(const int roomId, const char * roomName){
    for (int i=0; i<MAX_CLIENTS; i++){
        if (g_rooms[i].roomId == -1 || g_rooms[i].roomName[0] == '\0'){
            g_rooms[i].roomId = roomId;
            strcpy(g_rooms[i].roomName, roomName);

            Parameters params;
            char buffer[BUFFER];
            // online 
            strcpy(params.Param1, util_int_to_str(roomId)); 
            int len = generateMessage(3, 0, params, buffer);
            send_message(g_socket, buffer, len);
        }
    }
}

void in_member_list(const char * member, const int roomId){
    for (int i=0; i<10000; i++){
        if (g_room_member[i].memName[0] != '\0'){
            g_room_member[i].roomId = roomId;
            strcpy(g_room_member[i].memName, member);
        }
    }
}

void in_conversation(const int roomId, const char * timestamp, const char * username){
    for (int i=0; i<1024; i++){
        if (g_message[i].timestamp[0] == '\0'){
            g_message[i].roomId = roomId;
            strcpy(g_message[i].timestamp, timestamp);
            strcpy(g_message[i].userId, username);
        }
    }
}

void in_message(const int roomId, const char * timestamp, const char * message){
    for (int i=0; i<1024; i++){
        if (!strcmp(g_message[i].timestamp, timestamp) && roomId == g_message[i].roomId){
            strcpy(g_message[i].content, message);
        }
    }
}

void in_login_done(const char * username){
    // request initial data for running
    Parameters params;
    char buffer[BUFFER];
    // online 
    strcpy(params.Param1, util_int_to_str(g_rtd)); 
    int len = generateMessage(0, 0, params, buffer);
    send_message(g_socket, buffer, len);
    // friend 
    params.Param1[0]='\0';
    len = generateMessage(1, 0, params, buffer);
    send_message(g_socket, buffer, len);
    //room
    strcpy(params.Param1, username);    
    len = generateMessage(2, 0, params, buffer);
    send_message(g_socket, buffer, len);

}

void out_login(const char * username, const char * password){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, password); 
    int len = generateMessage(0, 1, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_register(const char * username, const char * password){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, password); 
    int len = generateMessage(0, 2, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_change_password(const char * username, const char * oldpass, const char * newpass){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, oldpass);
    strcpy(params.Param3, newpass); 
    int len = generateMessage(0, 3, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_friend_list(){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, "\0"); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(1, 0, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_sent_friend_request(const char * username, const char * friendname){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, friendname);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(1, 1, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_sent_friend_response(const char * username, const char * friendname){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, friendname);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(1, 2, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_room_list(const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 0, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_room_members(const int roomId){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, util_int_to_str(roomId)); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 1, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_create_room(const char * roomName, const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, roomName); 
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 2, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_add_member(const int roomId, const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, util_int_to_str(roomId)); 
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 3, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_remove_member(const int roomId, const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, util_int_to_str(roomId)); 
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 4, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_conversation(const int roomId){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, util_int_to_str(roomId)); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(3, 0, params, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_prev_conversation(const int roomId, const char * timestamp){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, util_int_to_str(roomId)); 
    strcpy(params.Param2, timestamp);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(3, 0, params, buffer);
    send_message(g_socket, buffer, len);
}


int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("Usage: %s ServerIPAddress PortNumber\n", argv[0]);
        exit(1);
    }
    char *clientIP = argv[1];
    int clientPort = atoi(argv[2]);

    run_client(clientIP,clientPort);

    //out_login("username", "password");
}