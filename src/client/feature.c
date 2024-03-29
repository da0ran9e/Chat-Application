#include "../../include/client/feature.h"

// Global variables are declared here to avoid multidefinition at `/usr/bin/ld`
int g_socket;
char g_address[15];
int g_port;
int g_rtd;
char g_username[50];
char g_user[MAX_CLIENTS][50];
int g_rtds[MAX_CLIENTS];
char g_friend[MAX_CLIENTS][50];
char g_request[MAX_CLIENTS][50];
Room g_rooms[MAX_CLIENTS];
Message g_message[BUFFER];
RoomMember g_room_member[MAX_CLIENTS * MAX_CLIENTS];
void *g_args;

void sendMessage(void *args, const char *buffer, int size)
{
    // printf("Send: ");
    // printCode(buffer, size);

    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;

    pthread_mutex_lock(&threadArgs->threadMutex);
    send(clientSocket, buffer, size, 0);

    pthread_mutex_unlock(&threadArgs->threadMutex);
    //usleep(PING_INTERVAL * 3000);
}

int recvAndProcess(void *args)
{
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER - 1, 0);
    buffer[bytesReceived] = '\0';

    // printf("Received: ");
    // printCode(buffer, bytesReceived+1);

    if (bytesReceived <= 0)
    {
        printf("Server disconnected.\n");
    }

    return handle_receive_message(buffer, bytesReceived);
}

void showFeatures()
{
    printf("\t---------------Start---------------\n");
    printf("\t0. Get online users\n");
    printf("\t1. Login\n");
    printf("\t2. Register\n");
    printf("\t3. Change password\n");
    printf("\t4. Friend list\n");
    printf("\t5. Request list\n");
    printf("\t6. Send friend request\n");
    printf("\t7. Response friend request\n");
    printf("\t8. Room list\n");
    printf("\t9. See members of room\n");
    printf("\t10. Create room\n");
    printf("\t11. Add user to room\n");
    printf("\t12. Remove a member\n");
    printf("\t13. Leave a room\n");
    printf("\t14. Conversation\n");
    printf("\t15. Send message\n");

    int choose;
    scanf("%d", &choose);
    switch (choose)
    {
    case 0:
        c_online();
        break;
    case 1:
        c_login();
        break;
    case 2:
        c_register();
        break;
    case 3:
        c_change_pass();
        break;
    case 4:
        c_friend_list();
        break;
    case 5:
        c_request_list();
        break; 
    case 6:
        c_send_friend_request();
        break;
    case 7:
        c_response_friend_request();
        break;
    case 8:
        c_rooms();
        break;
    case 9:
        c_members();
        break;
    case 10:
        c_create();
        break;
    case 11:
        c_add_member();
        break;
    case 12:
        c_remove_member();
        break;
    case 13:
        c_leave();
        break;
    case 14:
        c_conversation();
        break;
    case 15:
        c_chat();
        break;
    case 16:
        //c_load_all();
        break;
    default:
        break;
    }
}

void c_online()
{
    printf("\t---------------Online---------------\n");
    char buffer[BUFFER];
    Parameters params;

    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(0, 0, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 200)
        printf("Get online list!\n");
    else if (res == 300)
        printf("Incorrect user information!\n");
    else
    {
        printf("connection lost\n");
    }
}

void c_login()
{
    printf("\t---------------Login---------------\n");
    char buffer[BUFFER];
    Parameters params;

    printf("Enter username: ");
    scanf("%s", params.Param1);
    printf("Password for %s: ", params.Param1);
    scanf("%s", params.Param2);

    int len = generateMessage(0, 1, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 210)
    {
        printf("Login approved!\n");
        strcpy(g_username, params.Param1);
    }
    else if (res == 310)
        printf("Incorrect user information!\n");
    else
    {
        printf("Authentication failed! Please login again!\n");
    }
}

void c_register()
{
    printf("\t---------------Register---------------\n");
    char buffer[BUFFER];
    Parameters params;

    printf("Enter username: ");
    scanf("%s", params.Param1);
    printf("Password for %s: ", params.Param1);
    scanf("%s", params.Param2);

    int len = generateMessage(0, 2, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 220)
        printf("Create new account done!\n");
    else if (res == 320)
        printf("Username: %s already taken!\n", params.Param1);
    else
    {
        printf("Connection lost!\n");
    }
}

void c_change_pass()
{
    printf("\t---------------Change password---------------\n");
    char buffer[BUFFER];
    Parameters params;
    char temp[50];

    printf("Enter username: ");
    scanf("%s", params.Param1);
    printf("Enter password: ");
    scanf("%s", params.Param2);
    printf("New password for %s: ", params.Param1);
    scanf("%s", params.Param3);
    printf("Confirm new password: ");
    scanf("%s", temp);
    if (strcmp(params.Param3, temp))
    {
        printf("Confirm password does not match!\n");
    }
    else
    {
        int len = generateMessage(0, 3, params, buffer);
        sendMessage(g_args, buffer, len);

        int res = recvAndProcess(g_args);
        if (res == 230)
            printf("Change done!\n");
        else if (res == 330)
            printf("Cannot change password!\n");
        else
        {
            printf("Connection lost!\n");
        }
    }
}

void c_friend_list()
{
    printf("\t---------------Friends---------------\n");
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(1, 0, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 201)
        printf("Get friend successfully!\n");
    else if (res == 301)
        printf("Please login first!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_request_list()
{
    printf("\t---------------Requests---------------\n");
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(1, 3, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 201)
        printf("Get request successfully!\n");
    else if (res == 301)
        printf("Please login first!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_send_friend_request()
{
    printf("\t---------------Send Addfriend request---------------\n");
    Parameters params;
    char buffer[BUFFER];

    if (strlen(g_username) <= 0)
        printf("Please login first!\n");
    strcpy(params.Param1, g_username);
    printf("Send addfriend request to: ");
    scanf("%s", params.Param2);
    strcpy(params.Param3, "300");

    int len = generateMessage(1, 1, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 211)
        printf("Request sent!\n");
    //else if (res == 311)
    //    printf("User: %s is not online!\n", params.Param2);
    else
    {
        printf("Connection lost!\n");
    }
}

void c_response_friend_request()
{
    printf("\t---------------Addfriend---------------\n");
    Parameters params;
    char buffer[BUFFER];

    printf("Friend name: ");
    scanf("%s", params.Param1);
    strcpy(params.Param2, g_username);
    printf("1.accept\n2.deny\nChoose response: ");
    int c;
    scanf("%d",&c);
    switch (c)
    {
    case 1:
        strcpy(params.Param3, "accept");
        break;
    case 2:
        strcpy(params.Param3, "deny");
        break;
    default:
        break;
    }
    

    int len = generateMessage(1, 2, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 221)
        printf("Friend added!\n");
    else if (res == 321)
        printf("Friend request deleted!\n");
    else
    {
        printf("No request/Already friend!\n");
    }
}

void c_rooms()
{
    printf("\t---------------Room list---------------\n");
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, g_username);
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 0, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 202)
        printf("Get room successfully!\n");
    else if (res == 302)
        printf("Please login first!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_members()
{
    printf("\t---------------Member list---------------\n");
    Parameters params;
    char buffer[BUFFER];

    printf("Room id: ");
    scanf("%s", params.Param1);
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 1, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 212)
        printf("Get member successfully!\n");
    else if (res == 312)
        printf("Please login first!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_create()
{
    printf("\t---------------Create room---------------\n");
    Parameters params;
    char buffer[BUFFER];
    getchar();
    printf("Room name: ");
    fgets(params.Param1, 50, stdin);
    params.Param1[strlen(params.Param1) - 1] = '\0';
    strcpy(params.Param2, g_username);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 2, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 222)
        printf("Create room successfully!\n");
    else if (res == 322)
        printf("Please login first!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_add_member()
{
    printf("\t---------------Add member---------------\n");
    Parameters params;
    char buffer[BUFFER];

    printf("Room id: ");
    scanf("%s", params.Param1);
    printf("Member name: ");
    scanf("%s", params.Param2);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 3, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 232)
        printf("Member added!\n");
    else if (res == 332)
        printf("Not found user!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_remove_member()
{
    printf("\t---------------Remove member---------------\n");
    Parameters params;
    char buffer[BUFFER];

    printf("Room id: ");
    scanf("%s", params.Param1);
    printf("Member name: ");
    scanf("%s", params.Param2);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 4, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 242)
        printf("Member removed!\n");
    else if (res == 342)
        printf("User not found!\n");
    else
    {
        printf("Connection lost!\n");
    }
}


void c_leave()
{
    printf("\t---------------Leave A Room---------------\nPlease Login first!\n");
    Parameters params;
    char buffer[BUFFER];

    printf("Room id: ");
    scanf("%s", params.Param1);
    strcpy(params.Param2, g_username);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 4, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 242)
        printf("You have left the room!\n");
    else if (res == 342)
        printf("You are not in the room!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_conversation()
{
    printf("\t---------------Conversation---------------\n");
    Parameters params;
    char buffer[BUFFER];

    printf("Room id: ");
    scanf("%s", params.Param1);
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(3, 0, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 203)
        printf("Get conversation successfully!\n");
    else if (res == 303)
        printf("Room not found!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void c_chat()
{
    printf("\t---------------Send message---------------\n");
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, g_username);
    printf("Room id: ");
    scanf("%s", params.Param2);
    getchar(); // read the newline
    printf("Message:\n");
    fgets(params.Param3, 500, stdin);
    params.Param3[strlen(params.Param3) - 1] = '\0';

    int len = generateMessage(3, 1, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 213)
        printf("Sent!\n");
    else if (res == 313)
        printf("Room not found!\n");
    else
    {
        printf("Connection lost!\n");
    }
}

void ca_load_all(char *jsonStr){
    struct timeval startTime, endTime;
    gettimeofday(&startTime, NULL);

    /* Friend list */    
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(1, 0, params, buffer);
    sendMessage(g_args, buffer, len);
    int res = recvAndProcess(g_args);
    /* Online list */
    
    len = generateMessage(0, 0, params, buffer);
    sendMessage(g_args, buffer, len);
    res = recvAndProcess(g_args);

    /* Request list */
    
    len = generateMessage(1, 3, params, buffer);
    sendMessage(g_args, buffer, len);
    res = recvAndProcess(g_args);
    /* Room list */
    
    strcpy(params.Param1, g_username);

    len = generateMessage(2, 0, params, buffer);
    sendMessage(g_args, buffer, len);
    res = recvAndProcess(g_args);

    for (int i=0; i<MAX_CLIENTS; i++){
        if (g_rooms[i].roomId != -1){
            strcpy(params.Param1, util_int_to_str(g_rooms[i].roomId));

            len = generateMessage(2, 1, params, buffer);
            sendMessage(g_args, buffer, len);
            res = recvAndProcess(g_args);

            len = generateMessage(3, 0, params, buffer);
            sendMessage(g_args, buffer, len);
            res = recvAndProcess(g_args);
        }
    }
    gettimeofday(&endTime, NULL);
    long int roundTripTime = (endTime.tv_sec - startTime.tv_sec) * 100L +
                                 (endTime.tv_usec - startTime.tv_usec);

    /* Print */
    g_rtd = (int) roundTripTime;

    char jsonFrac[100][100];
    int count = 0;
    sprintf(jsonFrac[++count], "{\n");
    sprintf(jsonFrac[++count], "\t\"PORT\": %d,\n", g_port);
    sprintf(jsonFrac[++count], "\t\"rtd\": %d,\n", g_rtd);
    sprintf(jsonFrac[++count], "\t\"username\": \"%s\",\n", g_username);

    int fStart = 0;
    sprintf(jsonFrac[++count], "\t\"friends\": [\n");
    for (int i=0; i<MAX_CLIENTS; i++){
                if (g_friend[i][0] != '\0'){
                    if (fStart > 0) sprintf(jsonFrac[++count], ",\n"); 
                    sprintf(jsonFrac[++count], "\t\t{\"name\": \"%s\"}",g_friend[i]);
                    fStart++;
                }
            }
    sprintf(jsonFrac[++count], "\n\t],\n");

    int oStart=0;
    sprintf(jsonFrac[++count], "\t\"onlineUser\": [\n");
    for (int i=0; i<MAX_CLIENTS; i++){
                if (g_user[i][0] != '\0'){
                    if (oStart > 0) sprintf(jsonFrac[++count], ",\n"); 
                    int fr = 0;
                    for (int j=0; j<MAX_CLIENTS; j++){
                        if(g_friend[i][0]!='\0') fr++;
                    }
                    
                    if(fr) sprintf(jsonFrac[++count], "\t\t{\"name\": \"%s\", \"friend\":true}",g_user[i]);
                    else sprintf(jsonFrac[++count], "\t\t{\"name\": \"%s\", \"friend\":false}",g_user[i]);
                    oStart++;
                }
            }
    sprintf(jsonFrac[++count], "\n\t],\n");

    int rStart=0;
    sprintf(jsonFrac[++count], "\t\"requests\": [\n");
    for (int i=0; i<MAX_CLIENTS; i++){
                if (g_request[i][0] != '\0'){
                    if (rStart > 0) sprintf(jsonFrac[++count], ",\n"); 
                    sprintf(jsonFrac[++count], "\t\t{\"name\": \"%s\"}",g_request[i]);
                    rStart++;
                }
            }
    sprintf(jsonFrac[++count], "\n\t],\n");

    sprintf(jsonFrac[++count], "\t\"chatRooms\": [\n");
    int start = 0;
    int visited[MAX_CLIENTS];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (g_rooms[i].roomId != -1&&visited[i]!=1) {
            if (start > 0) sprintf(jsonFrac[++count], ",\n"); 
            sprintf(jsonFrac[++count], "\t\t{\n");
            sprintf(jsonFrac[++count], "\t\t\t\"id\": \"%d\",\n", g_rooms[i].roomId);
            sprintf(jsonFrac[++count], "\t\t\t\"name\": \"%s\",\n", g_rooms[i].roomName);
            sprintf(jsonFrac[++count], "\t\t\t\"members\": [\n");

            int start1 = 0;
            for (int j = 0; j < MAX_CLIENTS; j++) {
                if (g_room_member[j].roomId == g_rooms[i].roomId) {
                    if (start1 > 0) sprintf(jsonFrac[++count], ",\n");
                    sprintf(jsonFrac[++count], "\t\t\t\t{\"memberName\": \"%s\"}", g_room_member[j].memName);
                    start1++;
                }
            }
            sprintf(jsonFrac[++count], "\n\t\t\t],\n");
            sprintf(jsonFrac[++count], "\t\t\t\"messages\": [\n");

            int start2=0;
            for (int j = 0; j < MAX_CLIENTS; j++) {
                if (g_message[j].roomId == g_rooms[i].roomId) {
                    if (start2 > 0){ sprintf(jsonFrac[++count], ",\n");}
                    sprintf(jsonFrac[++count], "\t\t\t\t{\n");
                    sprintf(jsonFrac[++count], "\t\t\t\t\t\"user\": \"%s\",\n", g_message[j].userId);
                    sprintf(jsonFrac[++count], "\t\t\t\t\t\"timestamp\": \"%s\",\n", g_message[j].timestamp);
                    sprintf(jsonFrac[++count], "\t\t\t\t\t\"content\": \"%s\"\n", g_message[j].content);
                    sprintf(jsonFrac[++count], "\t\t\t\t}");
                    start2++;
                }
            }
            sprintf(jsonFrac[++count], "\n\t\t\t]\n\t\t}");
            start++;
            visited[i]=1;
        }
    }
    sprintf(jsonFrac[++count], "\n\t]\n");
    sprintf(jsonFrac[++count], "}\n");

    for (int i=0; i<=count; i++) {
        strcat(jsonStr, jsonFrac[i]);
    }
    printf("%s", jsonStr);
}

void ca_create_room(const char *roomName){
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, roomName);
    strcpy(params.Param2, g_username);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 2, params, buffer);
    sendMessage(g_args, buffer, len);
    int res = recvAndProcess(g_args);
}

void ca_send_message(const int roomId, const char *message){
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, g_username);
    strcpy(params.Param2, util_int_to_str(roomId));
    strcpy(params.Param3, message);

    int len = generateMessage(3, 1, params, buffer);
    sendMessage(g_args, buffer, len);
    int res = recvAndProcess(g_args);
}

void ca_add_room_member(const int roomId, const char *user){
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, util_int_to_str(roomId));
    strcpy(params.Param2, user);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 3, params, buffer);
    sendMessage(g_args, buffer, len);
    int res = recvAndProcess(g_args);
}

void ca_remove_member(const int roomId, const char *user){
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, util_int_to_str(roomId));
    strcpy(params.Param2, user);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 4, params, buffer);
    sendMessage(g_args, buffer, len);
    int res = recvAndProcess(g_args);
}

void ca_send_request(const char *friendName){
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, g_username);
    strcpy(params.Param2, friendName);
    strcpy(params.Param3, "300");

    int len = generateMessage(1, 1, params, buffer);
    sendMessage(g_args, buffer, len);
    int res = recvAndProcess(g_args);
}

void ca_response_request(const char *friendName, const int resonse){
    Parameters params;
    char buffer[BUFFER];

    strcpy(params.Param1, friendName);
    strcpy(params.Param2, g_username);
    switch (resonse)
    {
    case 1:
        strcpy(params.Param3, "accept");
        break;
    case 2:
        strcpy(params.Param3, "deny");
        break;
    default:
        break;
    }
    

    int len = generateMessage(1, 2, params, buffer);
    sendMessage(g_args, buffer, len);
    int res = recvAndProcess(g_args);
}

int handle_receive_message(const char *message, int len)
{
    int status;
    int op = getProtocolOpcode(message);
    int func = getProtocolFunctionCode(message);
    int code = 10 * func + op;
    Parameters params;
    char payload[BUFFER - 8];
    int plSize = len - 8;

    getProtocolPayload(message, payload, plSize);
    getProtocolParameters(payload, &params);

    // printf("code: %d\n", code);
    // handle
    switch (code)
    {
    case 00:
        if (params.Param1[0] == '\0')
        {
            //printf("-------------Online list-------------\nUsername\tRTTs\n");
            status = 200;
            break;
        }
        else
        {
            in_online_list(params.Param1, atoi(params.Param2));
            int res = recvAndProcess(g_args);
            printf("%s\t%s\n",params.Param1,params.Param2);
        }
        status = 200;
        break;
    case 10:

        if (!strcmp(params.Param1, "error"))
        {
            status = 310;
        }
        else
        {
            status = 210;
            in_login_done(params.Param1);
        }
        break;
    case 20:
        if (!strcmp(params.Param1, "error"))
        {
            status = 320;
        }
        else
        {
            status = 220;
        }
        break;
    case 30:
        if (!strcmp(params.Param1, "error"))
        {
            status = 330;
        }
        else
        {
            status = 230;
        }
        break;
    case 01:
        if (!strcmp(params.Param1, "error"))
        {
            status = 301;
        }
        else
        {
            status = 201;
            if (params.Param1[0] == '\0')
            {
                //printf("\t-------------Online list-------------\n\tFriendname:\n");
                break;
            }

            in_friend_list(params.Param1);
            int res = recvAndProcess(g_args);
            printf("\t\t%s\n", params.Param1);
        }
        break;
    case 11:
        if (!strcmp(params.Param1, "error"))
        {
            status = 311;
        }
        else
        {
            status = 211;
            //printf("Request sent!");
        }
        break;
    case 21:
        if (!strcmp(params.Param1, "error"))
        {
            status = 321;
        }
        else if (!strcmp(params.Param1, "accept"))
        {
            status = 221;
        }
        else
        {
            status = 121;
        }
        break;
    case 31:
        if (!strcmp(params.Param1, "error"))
        {
            status = 301;
        }
        else
        {
            status = 201;
            if (params.Param1[0] == '\0')
            {
                //printf("\t-------------Friend Request list-------------\n\tUsername:\n");
                break;
            }

            in_request_list(params.Param1);
            int res = recvAndProcess(g_args);
            printf("\t\t%s\n", params.Param1);
        }
        break;
    case 02:
        if (!strcmp(params.Param1, "error"))
        {
            status = 302;
        }
        else
        {
            status = 202;
            if (params.Param1[0] == '\0')
            {
                //printf("\t-------------Room list-------------\n\tRoom ID\tRoom name\n");
                break;
            }

            in_room_list(atoi(params.Param1), params.Param2);
            int res = recvAndProcess(g_args);
            printf("\t%s\t\t%s\n", params.Param1, params.Param2);
        }
        break;
    case 12:
        if (!strcmp(params.Param1, "error"))
        {
            status = 312;
        }
        else
        {
            status = 212;
            if (params.Param1[0] == '\0')
            {
                //printf("\t-------------Member list of %s-------------\n\tMember name\n", params.Param2);
                break;
            }

            in_member_list(params.Param1, atoi(params.Param2));
            int res = recvAndProcess(g_args);
            printf("\t%s\n", params.Param1);
        }
        break;
    case 22:
        if (!strcmp(params.Param1, "error"))
        {
            status = 322;
        }
        else
        {
            status = 222;
        }
        break;
    case 32:
        if (!strcmp(params.Param1, "error"))
        {
            status = 332;
        }
        else
        {
            status = 232;
        }
        break;
    case 42:
        if (!strcmp(params.Param1, "error"))
        {
            status = 342;
        }
        else
        {
            status = 242;
        }
        break;
    case 03:
        if (!strcmp(params.Param1, "error"))
        {
            status = 303;
        }
        else
        {
            status = 203;
            if (params.Param1[0] == '\0')
            {
                // printf("\t-------------Room: %s-------------\n", params.Param1);
                break;
            }
            status = 203;
            in_conversation(atoi(params.Param1), params.Param2, params.Param3);
            int res = recvAndProcess(g_args);
            printf("\t%.*s\t",20, params.Param3);
            // printCode(params.Param3, strlen(params.Param3));
            printf("%s: %s\n", params.Param2, params.Param3+51);
            // printCode(params.Param3+51, strlen(params.Param3)-51);
            
        }
        break;
    case 13:
        if (!strcmp(params.Param1, "error"))
        {
            status = 313;
        }
        else
        {
            status = 213;
        }
        break;
        
    default:
        break;
    }
    return status;
}

void in_online_list(const char *username, const int rtd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_user[i][0] == '\0')
        {
            strcpy(g_user[i], username);
            // printf("User online: %s\n", username);
            g_rtds[i] = rtd;
            break;
        }
    }
    char buffer[BUFFER];
    Parameters params;
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(0, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

void in_friend_list(const char *username)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_friend[i][0] == '\0')
        {
            strcpy(g_friend[i], username);
            // printf("Friend: %s\n", username);
            break;
        }
    }
    char buffer[BUFFER];
    Parameters params;
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(1, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

void in_request_list(const char *username)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_request[i][0] == '\0')
        {
            strcpy(g_request[i], username);
            // printf("Request: %s\n", username);
            break;
        }
    }
    char buffer[BUFFER];
    Parameters params;
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(1, 3, params, buffer);
    sendMessage(g_args, buffer, len);
}

void in_room_list(const int roomId, const char *roomName)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_rooms[i].roomId == -1)
        {
            g_rooms[i].roomId = roomId;
            strcpy(g_rooms[i].roomName, roomName);
            // printf("Room: %d\tName:%s\n", roomId, roomName);
            break;
        }
    }
    char buffer[BUFFER];
    Parameters params;
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(2, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

void in_member_list(const char *member, const int roomId)
{
    for (int i = 0; i < 10000; i++)
    {
        if (g_room_member[i].roomId == -1)
        {
            g_room_member[i].roomId = roomId;
            strcpy(g_room_member[i].memName, member);
            // printf("Room: %d\tUser:%s\n", roomId, member);
            break;
        }
    }
    char buffer[BUFFER];
    Parameters params;
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(2, 1, params, buffer);
    sendMessage(g_args, buffer, len);
}

void in_conversation(const int roomId, const char *username, const char *messageStr)
{
    for (int i = 0; i < 1024; i++)
    {
        if (g_message[i].timestamp[0] == '\0')
        {
            g_message[i].roomId = roomId;
            char cont[500];
            char timeStm[20];
            sprintf(timeStm, "%.*s", 20, messageStr);
            //strcpy(cont, messageStr+50);
            //util_get_substring(messageStr, cont, 50, strlen(messageStr) - 50);
            strcpy(g_message[i].timestamp, timeStm);
            strcpy(g_message[i].userId, username);
            strcpy(g_message[i].content, messageStr+50);
            
            break;
        }
    }
    char buffer[BUFFER];
    Parameters params;
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(3, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

void in_login_done(const char *username)
{
    // request initial data for running
    Parameters params;
    char buffer[BUFFER];
    //usleep(5000);
}

// Function to send messages to the server
void *sendThread(void *args)
{
    g_args = args;
    char buffer[BUFFER];

    while (1)
    {
        showFeatures();
    }

    pthread_exit(NULL);
}

// Function to send ping messages to the server
void *sendPingMessages(void *args)
{
    struct timeval startTime, endTime;

    while (1)
    {
    }

    pthread_exit(NULL);
}

void run_client(const char *address, const int port){
    
    g_socket = -1;
    g_port = -1;
    g_rtd = 9999;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        g_user[i][0] = '\0';
        g_rtds[i] = 9999;
        g_friend[i][0] = '\0';
        g_request[i][0] = '\0';
        g_rooms[i].roomId = -1;
        g_message[i].roomId = -1;
        g_room_member[i].roomId = -1;
    }
    strcpy(g_address, address);
    g_port = port;
    g_socket = initializeClient(g_address, g_port);

    // Create thread arguments
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    threadArgs->clientSocket = g_socket;
    pthread_mutex_init(&threadArgs->threadMutex, NULL); // Initialize the mutex

    // Create threads for sending and receiving messages, and sending ping messages
    pthread_t sendThreadID, pingThreadID;
    if (pthread_create(&sendThreadID, NULL, sendThread, (void *)threadArgs) != 0 ||
        pthread_create(&pingThreadID, NULL, sendPingMessages, (void *)threadArgs) != 0)
    {
        perror("Error creating threads");
        close(g_socket);
        free(threadArgs);
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(sendThreadID, NULL);
    pthread_join(pingThreadID, NULL);

    // Clean up and destroy the mutex
    pthread_mutex_destroy(&threadArgs->threadMutex);
    close(g_socket);
    free(threadArgs);

}