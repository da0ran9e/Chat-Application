#include "../../include/server/feature.h"

int g_clientSockets[MAX_CLIENTS];
char g_clientNames[MAX_CLIENTS][50];
int g_rtds[MAX_CLIENTS]; //  round-trip delay (time) or RTT

int handle_features(const int user, int op, int func, const Parameters params){
    switch (10*func + op)
    {
    case 0: // op 0, func 0
        break;
    case 10: // op 0, func 1
        break;
    case 20: // op 0, func 2
        break;
    case 30: // op 0, func 3
        break;
    case 1:
        break;
    case 11:
        break;
    case 21:
        break;
    case 2:
        break;
    case 12:
        break;
    case 22:
        break;
    case 32:
        break;
    case 3:
        break;
    case 13:
        break;
    default:
        return -1;
        break;
    }
}
/************************************************
*           server-side feature 
*************************************************/
// send user online list 
int feat_online_list (const int clientSock, const int rtd){
    int count=-1;
    for (int i=0; i<MAX_CLIENTS; i++){
        if (g_clientSockets[i] == clientSock) {
            g_rtds[i] = rtd;
            count++;
        }
        if (g_clientNames[i][0] != '\0'){
            //serialize message
            char buffer[BUFFER];
            Parameters p;
            strcpy(p.Param1, g_clientNames[i][0]);
            p.Param2 = '\0';
            p.Param3 = '\0';
            int len = writeMessage(0, 0, p, buffer);

            //send message
            int sent = sendMessage(clientSock, buffer, len);
            count++;
        }
    }
    if (count>0) return 200;
    else if(count==0) return 300;
    else return 400;
}

//login
int feat_login (const int clientSock, const char * username, const char * password){
    int res = s_auth_handle_login(username, password);
    if (res == 210){
        //update user list
        for (int i=0; i<MAX_CLIENTS; i++){
            if(g_clientSockets[i] == clientSock){
                strcpy(g_clientNames[i], username);
            }
        }
        //sent response
        Parameters p;
        char time[20];
        char message[BUFFER];
        util_get_time(time);
        strcpy(p.Param1, time);
        int len = writeMessage(0, 1, p, message);
        sendMessage(clientSock, message, len);
    }
    else{
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(0, 1, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//register
int feat_register (const int clientSock, const char * username, const char * password){
    int res = s_auth_handle_register(username, password);
    if (res == 220){
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "succes");
        int len = writeMessage(0, 2, p, message);
        sendMessage(clientSock, message, len);
    }
    else{
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(0, 2, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//change password
int feat_change_password (const int clientSock, const char *username, const char * oldpass, const char * newpass){
    int res = s_auth_change_password(username, oldpass, newpass);
    if (res == 230){
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "succes");
        int len = writeMessage(0, 3, p, message);
        sendMessage(clientSock, message, len);
    }
    else{
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(0, 3, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//get friend list
int feat_friend_list (const int clientSock){
    char username[50];
    char friendList[MAX_CLIENTS][50];
    int count;
    //get username
    for (int i=0; i<MAX_CLIENTS; i++){
        if (g_clientSockets == clientSock) {
            strcpy(username, g_clientNames[i]);
        }
    }

    int res = s_rela_friend_list(username, friendList, count);
    //response to client
    if (res == 201){
        for (int i=0; i<count; i++){

            for (int j=0; j<MAX_CLIENTS; j++){
                //serialize message
                Parameters p;
                strcpy(p.Param1, friendList[i]);
                strcpy(p.Param2, "9999");
                if (!strcmp(friendList[i],g_clientNames[i])){
                    strcpy(p.Param2, util_int_to_str(g_rtds[i]));
                }
                char buffer[BUFFER];
                int len = writeMessage(1, 0, p, buffer);

                //send 
                int sent = sendMessage(clientSock, buffer, len);
            }
        }
    }
    else{
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(1, 0, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//request 
int feat_request_friend (int clientSock, const char * username, const char * destination){
    //get destination socket 
    int destSock = -1;
    for (int i=0; i<MAX_CLIENTS; i++){
        if (!strcmp(g_clientNames[i], destination)){
            destSock = g_clientSockets[i];
        }
    }
    if (destSock != -1){
        //send invitation
        char buffer[BUFFER];
        Parameters p;
        strcpy(p.Param1, username);
        int len = writeMessage (1, 2, p, buffer);
        sendMessage(destSock, buffer, len);
    }
    else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(1, 1, p, message);
        sendMessage(clientSock, message, len);
    }
    return destSock;
}

//response friend request
int feat_response_request (int clientSock, const char * username, const char * destination, const char * response){
    //get destination socket 
    int destSock = -1;
    for (int i=0; i<MAX_CLIENTS; i++){
        if (!strcmp(g_clientNames[i], destination)){
            destSock = g_clientSockets[i];
        }
    }
    if (destSock != -1){
        int room = -1;
        if(!strcmp(response, "accept")){
            s_rela_addfriendship(username, destination, room);
        }
        //send response
        char buffer1[BUFFER];
        Parameters p1;
        strcpy(p1.Param1, username);
        strcpy(p1.Param2, destination);
        strcpy(p1.Param3, response);
        int len1 = writeMessage (1, 1, p1, buffer1);
        sendMessage(destSock, buffer1, len1);
        // adding new room
        char buffer2[BUFFER];
        Parameters p2;
        strcpy(p2.Param1, util_int_to_str(room));
        strcpy(p2.Param2, destination);
        int len2 = writeMessage (2, 2, p1, buffer2);
        sendMessage(clientSock, buffer2, len2);//send to friend
        strcpy(p2.Param2, username);
        int len2 = writeMessage (2, 2, p1, buffer2);
        sendMessage(destSock, buffer2, len2); //send to client
    }
    else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(1, 1, p, message);
        sendMessage(clientSock, message, len);
    }
    return destSock;
}

// get room list
int feat_room_list (const int clientSock, const char * username){
    Room rooms[MAX_CLIENTS];
    int count=0;
    int res = s_room_list (username, rooms, count);
    if (res==202){
        for(int i=0; i<count; i++){
            //serialize message
            Parameters p;
            char buffer[BUFFER];
            strcpy(p.Param1, util_int_to_str(rooms[i]->roomId));
            strcpy(p.Param2, rooms[i]->roomName);
            int len = writeMessage(2, 0, p, buffer);
            sendMessage(clientSock, buffer, len);
        }
    }else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(2, 0, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

// get memberlist
int feat_room_members (const int clientSock, const int roomId){
    char peopleList[MAX_CLIENTS][50];
    int count=0;
    int res = s_room_members(roomId, peopleList, count);
    if (res == 212){
        //serialize message
        for (int i=0; i<count; i++){
            Parameters p;
            char buffer[BUFFER];
            strcpy(p.Param1, peopleList[i]);
            int len = writeMessage(2, 1, p, buffer);
            sendMessage(clientSock, buffer, len);
        }
    }else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(2, 1, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//create room
int feat_room_create(const int clientSock, const char name, const char * username){
    int roomId;
    int res = s_room_create(name, username, roomId);
    if (res==222){
        Parameters p;
        char buffer[BUFFER];
        strcpy(p.Param1, util_int_to_str(roomId));
        int len = writeMessage(2, 2, p, buffer);
        sendMessage(clientSock, buffer, len);
    }else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(2, 2, p, message);
        sendMessage(clientSock, message, len);
    }
}
/************************************************
*           server-side handler
*************************************************/

// process message before send to client
int writeMessage(const int op, const int func, const Parameters params, char * buffer){
    return generateMessage(op, func, params, buffer);
}

// process message from client
int readMessage(const char * buffer, const int size, Parameters params) {
    int op = getProtocolOpcode(buffer)
    int fun = getProtocolFunctionCode(buffer);
    char payload[size];

    getProtocolPayload(buffer, payload, sizeof(payload));

    params = getProtocolParameters(payload, p);

    printf("Opcode: %d\n", op);
    printf("Func: %d\n", func);
    printf("len1: %d\n", strlen(p.Param1));
    printf("Param1: %s\n", p.Param1);
    printf("len2: %d\n", strlen(p.Param2));
    printf("Param2: %s\n", p.Param2);
    printf("len3: %d\n", strlen(p.Param3));
    printf("Param3: %s\n", p.Param3);
    // // Print the binary string

    // printf("Binary String : \n");
    // for (size_t i = 0; i < size; i++) {
    //     printf("\\x%02X", (unsigned char)buffer[i]);
    // }
    // printf("\n");

    return 10*func+op;
}

// Function to handle I/O asynchronously in a thread
void *handleClient(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = receiveMessage(clientSocket, buffer);

        if (bytesReceived <= 0) {
            break; // Connection closed or error
        }

        // Process the received message
        printf("Processing message from client %d: %s\n", clientSocket, buffer);
        readMessage(buffer, sizeof(buffer));

        sendMessage(clientSocket, buffer, strlen(buffer));        // Echo the message back to the client
    }

    close(clientSocket);
    free(threadArgs); // Free the memory allocated for thread arguments
    pthread_exit(NULL);
}

// start server
void runServer(int serverSocket) {

    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    while (1) {
        // Accept the connection
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrLen);
        if (clientSocket == -1) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        // Add the new client socket to the array
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (g_clientSockets[i] == -1) {
                g_clientSockets[i] = clientSocket;
                break;
            }
        }
        printf("New connection from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
        connectionLog(CONNECT, ntohs(clientAddr.sin_port), inet_ntoa(clientAddr.sin_addr));
        //thread arguments
        struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
        threadArgs->clientSocket = clientSocket;

        pthread_t thread;// Create a new thread to handle the client
        if (pthread_create(&thread, NULL, handleClient, (void *)threadArgs) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
        pthread_detach(thread);// clean up resources
    }
    close(serverSocket);
}

// initial function 
void _init (int port){
    //initialize client list
    for (int i=0; i<MAX_CLIENTS; i++){
        g_clientSockets[i] = -1;
        g_clientNames[i][0] = '\0';
        g_rtds[i] = 9999;
    }
    int serverSocket;
    initializeServer(&serverSocket, port);
    runServer(serverSocket);
    serverLog(START, port);
}