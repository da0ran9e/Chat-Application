#include "../../include/server/feature.h"

int handle_features(const int userSock, int op, int func, const Parameters params){
    int result = 0;
    int code = 10*func + op;
    eventLog(code, g_port);
    switch (code)
    {
    case 0: // op 0, func 0
        result = feat_online_list(userSock, atoi(params.Param1));
        break;
    case 10: // op 0, func 1
        result = feat_login(userSock, params.Param1, params.Param2);
        break;
    case 20: // op 0, func 2
        result = feat_register(userSock, params.Param1, params.Param2);
        break;
    case 30: // op 0, func 3
        result = feat_change_password(userSock, params.Param1, params.Param2, params.Param3);
        break;
    case 1:
        result = feat_friend_list(userSock);
        break;
    case 11:
        result = feat_request_friend(userSock, params.Param1, params.Param2);
        break;
    case 21:
        result = feat_response_request(userSock, params.Param1, params.Param2, params.Param3);
        break;
    case 2:
        result = feat_room_list(userSock, params.Param1);
        break;
    case 12:
        result = feat_room_members(userSock, atoi(params.Param1));
        break;
    case 22:
        result = feat_room_create(userSock, params.Param1, params.Param2);
        break;
    case 32:
        result = feat_add_member(userSock, atoi(params.Param1), params.Param2);
        break;
    case 42:
        result = feat_remove_member (userSock, atoi(params.Param1), params.Param2);
        break;
    case 3:
        result = feat_conversation (userSock, atoi(params.Param1));
        break;
    case 13:
        result = feat_new_message (userSock, params.Param1, atoi(params.Param2), params.Param3);
        break;
    default:
        return -1;
        break;
    }
    return result;
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
            strcpy(p.Param1, g_clientNames[i]);
            strcpy(p.Param2, util_int_to_str(g_rtds[i]));
            p.Param3[0] = '\0';
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
                g_rtds[i] = 99;
                clientLog(LOGIN, username, g_ipAddr[i], g_port);
            }
        }
        //sent response
        Parameters p;
        char time[20];
        char message[BUFFER];
        util_get_time(time);
        strcpy(p.Param1, time);
        strcpy(p.Param2, username);
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
        strcpy(p.Param1, "success");
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
        strcpy(p.Param1, "success");
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
        if (g_clientSockets[i] == clientSock) {
            strcpy(username, g_clientNames[i]);
        }
    }
printf("username: %s\n", username);
    int res = s_rela_friend_list(username, friendList, &count);
    printf("res: %d\n", res);
    printf("count: %d\n", count);
    //response to client
    if (res == 201){
        for (int i=0; i< count; i++){


                printf("%s\n", friendList[i]);
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
    else{
        printf("people not found!\n");
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
        printf("found destination user at socket %d\n", destSock);
        //send invitation
        char buffer[BUFFER];
        Parameters p;
        strcpy(p.Param1, username);
        int len = writeMessage (1, 2, p, buffer);
        sendMessage(destSock, buffer, len);
    }
    else{
        printf("user not online!\n");
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
        printf("accept request!\n");
        int room = -1;
        if(!strcmp(response, "accept")){
            s_rela_addfriendship(username, destination, &room);
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
        len2 = writeMessage (2, 2, p1, buffer2);
        sendMessage(destSock, buffer2, len2); //send to client
    }
    else{
        printf("user not online!\n");
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
    int res = s_room_list (username, rooms, &count);
    if (res==202){
        //printf("res: %d\n", res);
        //printf("count: %d, 1st room: %s", count, rooms[0].roomName);
        for(int i=0; i<count; i++){
            printf("room %d found: %s \n", rooms[i].roomId, rooms[i].roomName);
            //serialize message
            Parameters p;
            char buffer[BUFFER];
            strcpy(p.Param1, util_int_to_str(rooms[i].roomId));
            strcpy(p.Param2, rooms[i].roomName);
            int len = writeMessage(2, 0, p, buffer);
            sendMessage(clientSock, buffer, len);
        }
    }else{
        printf("no room found!\n");
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
    int res = s_room_members(roomId, peopleList, &count);
    if (res == 212){
        //serialize message
        for (int i=0; i<count; i++){
            printf("people: %s at %d\n", peopleList[i], roomId);
            Parameters p;
            char buffer[BUFFER];
            strcpy(p.Param1, peopleList[i]);
            strcpy(p.Param2, util_int_to_str(roomId));
            int len = writeMessage(2, 1, p, buffer);
            sendMessage(clientSock, buffer, len);
        }
    }else{
        printf("people not found!\n");
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(2, 1, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//create room
int feat_room_create(const int clientSock, const char * roomName, const char * username){
    int roomId;
    int res = s_room_create(roomName, username, &roomId);
    if (res==222){
        Parameters p;
        char buffer[BUFFER];
        strcpy(p.Param1, util_int_to_str(roomId));
        strcpy(p.Param2, roomName);
        int len = writeMessage(2, 2, p, buffer);
        sendMessage(clientSock, buffer, len);
    }else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(2, 2, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//add members
int feat_add_member(const int clientSock, const int roomId, const char * username) {
    int res = s_room_add_member(roomId, username);
    if (res == 232){
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, util_int_to_str(roomId));
        strcpy(p.Param2, username);
        int len = writeMessage(2, 3, p, message);
        sendMessage(clientSock, message, len);
    }
    else{
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(2, 3, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}

//remove
int feat_remove_member(const int clientSock, const int roomId, const char * username){
    int res = s_room_remove_member(roomId, username);
    if (res == 242){
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, util_int_to_str(roomId));
        strcpy(p.Param2, username);
        int len = writeMessage(2, 4, p, message);
        sendMessage(clientSock, message, len);
    }
    else{
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(2, 4, p, message);
        sendMessage(clientSock, message, len);
    }
    return res;
}   

//get conversation
int feat_conversation (const int clientSock, const int roomId){
    int count;
    char conv[100][50];
    int res = s_conv_get_conversation(roomId, NULL, conv, &count);
    printf("res: %d, count: %d\n", res, count);
    if (res==203){
        
        if (count == 0){
            Parameters p;
            char buffer[BUFFER];
            strcpy(p.Param1, "success");
            int len = writeMessage(3, 0, p, buffer);
            sendMessage(clientSock, buffer, len);
        }
        else{
            for (int i=0; i<count; i++){
                Parameters p;
                char buffer[BUFFER];
                Parameters p1;
                char buffer2[BUFFER];
                Message m;
                //printf("util int to str: %s\n",util_int_to_str(roomId));
                strcpy(p.Param1, util_int_to_str(roomId));
                //printf("conv[i]: %s\n",conv[i]);
                strcpy(p.Param2, conv[i]);
                res = s_conv_get_message(roomId, conv[i], &m);
                strcpy(p.Param3, m.userId);
                printf("m user: %s\n", m.userId);
                printf("m time: %s\n", m.timestamp);
                printf("m content: %s\n",m.content);  
                int len = writeMessage(3, 0, p, buffer);
                sendMessage(clientSock, buffer, len);

                strcpy(p1.Param1, util_int_to_str(roomId));
                strcpy(p1.Param2, m.timestamp);
                strcpy(p1.Param3, m.content);

                int len2 = writeMessage(3, 1, p1, buffer2);
                sendMessage(clientSock, buffer2, len2);
            }     
        }
    }else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(3, 0, p, message);
        sendMessage(clientSock, message, len);  
    }
    return res;
}

int feat_load_more(const int clientSock, const int roomId, const char * timestamp){
    int count;
    char conv[100][50];
    int res = s_conv_get_conversation(roomId, timestamp, conv, &count);
    if (res==203){
        
        if (count == 0){
            Parameters p;
            char buffer[BUFFER];
            strcpy(p.Param1, "success");
            int len = writeMessage(3, 0, p, buffer);
            sendMessage(clientSock, buffer, len);
        }
        else{
            for (int i=0; i<count; i++){
                Parameters p1;
                char buffer1[BUFFER];
                Parameters p2;
                char buffer2[BUFFER];
                Message *m;

                strcpy(p1.Param1, util_int_to_str(roomId));
                strcpy(p1.Param2, conv[i]);
                res = s_conv_get_message(roomId, timestamp, m);
                strcpy(p1.Param3, m->userId);

                int len1 = writeMessage(3, 0, p1, buffer1);
                sendMessage(clientSock, buffer1, len1);

                strcpy(p2.Param1, util_int_to_str(roomId));
                strcpy(p2.Param2, m->timestamp);
                strcpy(p2.Param3, m->content);
                int len2 = writeMessage(3, 1, p2, buffer2);
                sendMessage(clientSock, buffer2, len2);
            }     
        }
    }else{
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "error");
        int len = writeMessage(3, 0, p, message);
        sendMessage(clientSock, message, len);  
    }
    return res;
}

// send message
int feat_new_message(const int clientSock, const char * username, const int roomId, const char * message){
    int res = s_conv_new_message(username, roomId, message);
    if (res == 213){
        //sent response
        Parameters p;
        char message[BUFFER];
        strcpy(p.Param1, "success");
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
/************************************************
*           server-side handler
*************************************************/

// process message before send to client
int writeMessage(const int op, const int func, const Parameters params, char * buffer){
    return generateMessage(op, func, params, buffer);
}

// // process message from client
// int readMessage(const char * buffer, const int size, Parameters params) {
//     int op = getProtocolOpcode(buffer);
//     int func = getProtocolFunctionCode(buffer);
//     char payload[size];

//     getProtocolPayload(buffer, payload, sizeof(payload));

//     getProtocolParameters(payload, params);

//     // printf("Opcode: %d\n", op);
//     // printf("Func: %d\n", func);
//     // printf("len1: %d\n", strlen(params.Param1));
//     // printf("Param1: %s\n", params.Param1);
//     // printf("len2: %d\n", strlen(params.Param2));
//     // printf("Param2: %s\n", params.Param2);
//     // printf("len3: %d\n", strlen(params.Param3));
//     // printf("Param3: %s\n", params.Param3);
//     // // Print the binary string

//     // printf("Binary String : \n");
//     // for (size_t i = 0; i < size; i++) {
//     //     printf("\\x%02X", (unsigned char)buffer[i]);
//     // }
//     // printf("\n");

//     return 10*func+op;
// }

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
        int op = getProtocolOpcode(buffer);
        int func = getProtocolFunctionCode(buffer);
        Parameters params;
        int plSize = bytesReceived;
        char payload[plSize];
        getProtocolPayload(buffer, payload, plSize);
        getProtocolParameters(payload, &params);
        //readMessage(buffer, sizeof(buffer), p);
        handle_features(clientSocket, op, func, params);

        sendMessage(clientSocket, buffer, bytesReceived);        // Echo the message back to the client
    }
    for (int i=0; i<MAX_CLIENTS; i++){
        if (g_rtds[i] < 9999){
            g_rtds[i] = 9999;
            g_clientNames[i][0] = '\0';
            g_clientSockets[i] = -1;
            clientLog(LOGOUT, g_clientNames[i], g_ipAddr[i], g_port);
        }
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
                strcpy(g_ipAddr[i], inet_ntoa(clientAddr.sin_addr));
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