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

#define BUFFER 1024
#define PING_INTERVAL 350 // Interval in seconds for sending ping messages

#define MAX_CLIENTS 100

enum RequestEvent
{
    REQ_PING = 00,
    REQ_LOGIN = 10,
    REQ_REGISTER = 20,
    REQ_CHANGE_PASS = 30,
    REQ_FRIEND_LIST = 01,
    REQ_FRIEND_REQUEST = 11,
    REQ_RESPONSE_REQUEST = 21,
    REQ_ROOM_LIST = 02,
    REQ_ROOM_MEMBER = 12,
    REQ_CREATE_ROOM = 22,
    REQ_ADD_MEMBER = 32,
    REQ_REMOVE_MEMBER = 42,
    REQ_CONVERSATION = 03,
    REQ_MESSAGE = 13,
};

typedef struct RoomMember
{
    int roomId;
    char memName[50];
    // memName[0] = '\0';
} RoomMember;

typedef struct Room
{
    int roomId;
    char roomName[50];
} Room;

typedef struct Message
{
    int roomId;
    char userId[50]; // username
    char timestamp[50];
    char content[500];
} Message;

int g_socket;
char g_address[15];
int g_port;
int g_rtd;
char g_username[50];
char g_user[MAX_CLIENTS][50];
int g_rtds[MAX_CLIENTS];
char g_friend[MAX_CLIENTS][50];
Room g_rooms[MAX_CLIENTS];
Message g_message[BUFFER];
RoomMember g_room_member[MAX_CLIENTS * MAX_CLIENTS];
void *g_args; // forbidden declaration =))

// utility////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Function to convert an integer to a string
char *util_int_to_str(int value)
{
    char *str = (char *)malloc(15); // a maximum of 12 digits for an int
    snprintf(str, 15, "%d", value);
    return str;
}

// get current timestamp as string (format: YYYY-mm-dd HH:MM:SS)
void util_get_time(char *timeString)
{
    time_t currentTime = time(NULL);

    if (currentTime == -1)
    {
        perror("Error getting current time");
        return;
    }
    struct tm *timeInfo = localtime(&currentTime);
    strftime(timeString, 20, "%Y-%m-%d %H:%M:%S", timeInfo);
}

// get a substring from a binary string
void util_get_substring(const char *string, char *buffer, int pos, int len)
{
    char *temp = (char *)malloc((len + 1) * sizeof(char));
    strcpy(temp, string + pos);
    temp[len] = '\0'; // Null-terminate the string
    strcpy(buffer, temp);
}
///////////////////////////////////////

typedef struct Parameters
{
    char Param1[1024];
    char Param2[1024];
    char Param3[1024];
} Parameters;

uint32_t getProtocolOpcode(const char *message);
uint32_t getProtocolFunctionCode(const char *message);
void getProtocolPayload(const char *message, char *buffer, int size);
void getProtocolParameters(const char *payload, Parameters *parameters);
int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char *buffer);
void printCode(const char *binaryStr, size_t size);
/////////////////////////////////////////
void in_online_list(const char *username, const int rtd);
void in_friend_list(const char *username);
void in_room_list(const int roomId, const char *roomName);
void in_member_list(const char *member, const int roomId);
void in_conversation(const int roomId, const char *timestamp, const char *username);
void in_message(const int roomId, const char *timestamp, const char *message);
void in_login_done(const char *username);
void out_login(const char *username, const char *password);
void out_register(const char *username, const char *password);
void out_change_password(const char *username, const char *oldpass, const char *newpass);
void out_get_friend_list();
void out_sent_friend_request(const char *username, const char *friendname);
void out_sent_friend_response(const char *username, const char *friendname);
void out_get_room_list(const char *username);
void out_get_room_members(const int roomId);
void out_create_room(const char *roomName, const char *username);
void out_add_member(const int roomId, const char *username);
void out_remove_member(const int roomId, const char *username);
void out_get_conversation(const int roomId);
int handle_receive_message(const char *message, int len);
/////////////////////////////////////////////

uint32_t getProtocolOpcode(const char *message)
{
    // Extract the integer from the first 4 bytes
    uint32_t opCode;
    memcpy(&opCode, message, sizeof(opCode));
    return opCode;
}

void printCode(const char *binaryStr, size_t size)
{

    for (size_t i = 0; i < size; i++)
    {
        printf("\\x%02X", (unsigned char)binaryStr[i]);
    }
    printf("\n");
}

uint32_t getProtocolFunctionCode(const char *message)
{
    // Extract the integer from the next 4 bytes
    uint32_t func;
    memcpy(&func, message + sizeof(uint32_t), sizeof(func));
    return func;
}

void getProtocolPayload(const char *message, char *buffer, int size)
{
    // Extract the payload from the entire of the message
    memcpy(buffer, message + 2 * sizeof(uint32_t), size);
}

void getProtocolParameters(const char *payload, Parameters *parameters)
{
    // Extract parameters from the payload
    uint32_t paramLen1, paramLen2, paramLen3;
    memcpy(&paramLen1, payload, sizeof(paramLen1));
    memcpy(&paramLen2, payload + sizeof(uint32_t) + paramLen1, sizeof(paramLen2));
    memcpy(&paramLen3, payload + 2 * sizeof(uint32_t) + paramLen1 + paramLen2, sizeof(paramLen3));

    // printf("paramlen1: %d\n", paramLen1);
    // printf("paramlen2: %d\n", paramLen2);
    // printf("paramlen3: %d\n", paramLen3);

    util_get_substring(payload, parameters->Param1, 4, paramLen1);
    util_get_substring(payload, parameters->Param2, 8 + paramLen1, paramLen2);
    util_get_substring(payload, parameters->Param3, 12 + paramLen1 + paramLen2, paramLen3);
}

int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char *buffer)
{
    int len1 = strlen(parameters.Param1);
    int len2 = strlen(parameters.Param2);
    int len3 = strlen(parameters.Param3);
    printf("len1: %d\n", len1);
    printf("len2: %d\n", len2);
    printf("len3: %d\n", len3);

    printf("Param1: %s\n", parameters.Param1);
    printf("Param2: %s\n", parameters.Param2);
    printf("Param3: %s\n", parameters.Param3);

    int bufferSize = 20 + len1 + len2 + len3;

    memcpy(buffer, &op, sizeof(op));
    memcpy(buffer + 4, &func, sizeof(func));
    memcpy(buffer + 8, (uint32_t *)&len1, sizeof(uint32_t));
    memcpy(buffer + 12, parameters.Param1, len1 * sizeof(uint32_t));
    memcpy(buffer + 12 + len1, (uint32_t *)&len2, sizeof(uint32_t));
    memcpy(buffer + 16 + len1, parameters.Param2, len2 * sizeof(uint32_t));
    memcpy(buffer + 16 + len1 + len2, (uint32_t *)&len3, sizeof(uint32_t));
    memcpy(buffer + 20 + len1 + len2, parameters.Param3, len3 * sizeof(uint32_t));
    buffer[bufferSize] = '\0';
    return bufferSize;
}
///////////////////////////////////////////////////////////////////////////////////
void showFeatures();
void c_online();
void c_login();
void c_register();
void c_change_pass();
void c_friend_list();
void c_send_friend_request();
void c_response_friend_request();
void c_rooms();
void c_members();
void c_create();
void c_add_member();
void c_remove_member();
void c_conversation();
void c_chat();
// Structure to pass arguments to the thread
struct ThreadArgs
{
    int clientSocket;
    pthread_mutex_t threadMutex;
};

void sendMessage(void *args, const char *buffer, int size)
{
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;

    // Acquire the mutex lock before calling send method
    pthread_mutex_lock(&threadArgs->threadMutex);
    send(clientSocket, buffer, size, 0);
    // Release the mutex lock after calling send method
    pthread_mutex_unlock(&threadArgs->threadMutex);
    usleep(PING_INTERVAL * 3000);
}

int recvAndProcess(void *args)
{
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    memset(buffer, 0, sizeof(buffer));

    ssize_t bytesReceived = recv(clientSocket, buffer, BUFFER - 1, 0);
    if (bytesReceived <= 0)
    {
        printf("Server disconnected.\n");
    }

    return handle_receive_message(buffer, bytesReceived);
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

void showFeatures()
{
    printf("---------------Start---------------\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Change password\n");
    printf("4. Friend list\n");
    printf("5. Send friend request\n");
    printf("6. Response friend request\n");
    printf("7. Room list\n");
    printf("8. See members of room\n");
    printf("9. Create room\n");
    printf("10. Add user to room\n");
    printf("11. Remove a member\n");
    printf("12. Conversation\n");
    printf("13. Send message\n");

    int choose;
    scanf("%d", &choose);
    switch (choose)
    {
        case 0:
        c_online();
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
        c_send_friend_request();
        break;
    case 6:
        c_response_friend_request();
        break;
    case 7:
        c_rooms();
        break;
    case 8:
        c_members();
        break;
    case 9:
        c_create();
        break;
    case 10:
        c_add_member();
        break;
    case 11:
        c_remove_member();
        break;
    case 12:
        c_conversation();
        break;
    case 13:
        c_chat();
        break;
    default:
        break;
    }
}

void c_online(){
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
    printf("Password for %s:", params.Param1);
    scanf("%s", params.Param2);

    int len = generateMessage(0, 1, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 210)
        printf("Login approved!\n");
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
    printf("Password for %s:", params.Param1);
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
        printf("Get friend list successfully!\n");
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
    else if (res == 311)
        printf("User: %s is not online!\n", params.Param2);
    else
    {
        printf("Connection lost!\n");
    }
}

void c_response_friend_request()
{
    printf("\t---------------Response Addfriend---------------\n");
    Parameters params;
    char buffer[BUFFER];

    printf("Response request from: ");
    scanf("%s", params.Param1);
    strcpy(params.Param2, g_username);
    printf("Response (accept/deny): ");
    scanf("%s", params.Param3);

    int len = generateMessage(1, 2, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 221)
        printf("Request sent!\n");
    else if (res == 321)
        printf("User: %s is not online!\n", params.Param2);
    else
    {
        printf("Connection lost!\n");
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
        printf("Get rooms successfully!\n");
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

    printf("Room id:");
    scanf("%s", params.Param1);
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 1, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 212)
        printf("Get member list successfully!\n");
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

    printf("Room name:");
    scanf("%s", params.Param1);
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

    printf("Room id:");
    scanf("%s", params.Param1);
    printf("Member name:");
    scanf("%s", params.Param2);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 3, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 232)
        printf("Member removed!\n");
    else if (res == 332)
        printf("Member not here!\n");
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

    printf("Room id:");
    scanf("%s", params.Param1);
    printf("Member name:");
    scanf("%s", params.Param2);
    strcpy(params.Param3, "\0");

    int len = generateMessage(2, 4, params, buffer);
    sendMessage(g_args, buffer, len);

    int res = recvAndProcess(g_args);
    if (res == 242)
        printf("Added!\n");
    else if (res == 342)
        printf("User not found!\n");
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

    printf("Room id:");
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
    printf("Room id:");
    scanf("%s", params.Param2);
    printf("Message:\n");
    fgets(params.Param3, 500, stdin);

    int len = generateMessage(3, 0, params, buffer);
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

// Function to receive messages from the server
void *receiveThread(void *args)
{
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int clientSocket = threadArgs->clientSocket;
    char buffer[BUFFER];

    while (1)
    {

    }

    pthread_exit(NULL);
}

// Function to send ping messages to the server
void *sendPingMessages(void *args)
{
    struct timeval startTime, endTime;

    while (1)
    {
        // sleep(30);
        // gettimeofday(&startTime, NULL);

        // // Send a ping message to the server
        // const char *pingMessage = "PING";
        // sendMessage(g_args, pingMessage, 4);
        // // send(clientSocket, pingMessage, strlen(pingMessage), 0);

        // // Calculate and print round-trip delay time
        // gettimeofday(&endTime, NULL);
        // long int roundTripTime = (endTime.tv_sec - startTime.tv_sec) * 1000000L +
        //                          (endTime.tv_usec - startTime.tv_usec);
        // printf("%ld microseconds\n", roundTripTime);
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s ServerIPAddress PortNumber\n", argv[0]);
        exit(1);
    }

    int clientSocket;
    struct sockaddr_in serverAddr;

    // Create a socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating client socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &serverAddr.sin_addr) <= 0)
    {
        perror("Invalid server address");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Error connecting to the server");
        close(clientSocket);
        exit(EXIT_FAILURE);
    }

    g_socket = -1;
    g_port = -1;
    g_rtd = 9999;
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        g_user[i][0] = '\0';
        g_rtds[i] = 9999;
        g_friend[i][0] = '\0';
        g_rooms[i].roomId = -1;
    }
    strcpy(g_address, argv[1]);
    g_port = atoi(argv[2]);

    // Create thread arguments
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
    threadArgs->clientSocket = clientSocket;
    pthread_mutex_init(&threadArgs->threadMutex, NULL); // Initialize the mutex

    // Create threads for sending and receiving messages, and sending ping messages
    pthread_t sendThreadID, receiveThreadID, pingThreadID;
    if (pthread_create(&sendThreadID, NULL, sendThread, (void *)threadArgs) != 0 ||
        pthread_create(&receiveThreadID, NULL, receiveThread, (void *)threadArgs) != 0 ||
        pthread_create(&pingThreadID, NULL, sendPingMessages, (void *)threadArgs) != 0)
    {
        perror("Error creating threads");
        close(clientSocket);
        free(threadArgs);
        exit(EXIT_FAILURE);
    }

    // Wait for threads to finish
    pthread_join(sendThreadID, NULL);
    pthread_join(receiveThreadID, NULL);
    pthread_join(pingThreadID, NULL);

    // Clean up and destroy the mutex
    pthread_mutex_destroy(&threadArgs->threadMutex);
    close(clientSocket);
    free(threadArgs);

    return 0;
}

// features/////////////////////////////////////////////////////////////////////////////

int handle_receive_message(const char *message, int len)
{
    printf("received: ");
    printCode(message, len);

    int status;
    int op = getProtocolOpcode(message);
    int func = getProtocolFunctionCode(message);
    int code = 10 * func + op;
    Parameters params;
    char payload[BUFFER - 8];
    int plSize = 0;

    getProtocolPayload(message, payload, plSize);
    getProtocolParameters(payload, &params);

    // printf("code: %d\n", code);
    // handle
    switch (code)
    {
    case 00:
        in_online_list(params.Param1, atoi(params.Param2));
        //out_get_friend_list();
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
            in_friend_list(params.Param1);
            //out_get_room_list(g_username);
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
    case 02:
        if (!strcmp(params.Param1, "error"))
        {
            status = 302;
        }
        else
        {
            status = 202;
            in_room_list(atoi(params.Param1), params.Param2);
            //out_get_room_members(atoi(params.Param1));
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
            in_member_list(params.Param1, atoi(params.Param2));
            //out_get_conversation(atoi(params.Param2));
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
            in_room_list(atoi(params.Param1), "\0");
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
            in_conversation(atoi(params.Param1), params.Param2, params.Param3);
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
            in_message(atoi(params.Param1), params.Param2, params.Param3);
        }
        break;
    default:
        break;
    }
    return status;
}

void in_online_list(const char *username, const int rtd)
{
    printf("//////////////////////////////////////in online\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_user[i][0] != '\0')
        {
            strcpy(g_user[i], username);
            g_rtds[i] = rtd;
        }
    }
    printf("//////////////////////////////////////continue online\n");
    char buffer[BUFFER];
        Parameters params;
        strcpy(params.Param1, util_int_to_str(g_rtd));
        strcpy(params.Param2, "\0");
        strcpy(params.Param3, "\0");
        int len = generateMessage(0, 0, params, buffer);
        sendMessage(g_args, buffer, len);
}

void in_friend_list(const char *username)
{
    printf("//////////////////////////////////////in friends\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_friend[i][0] != '\0')
        {
            strcpy(g_friend[i], username);
        }
    }
    printf("//////////////////////////////////////continue friend\n");
    char buffer[BUFFER];
        Parameters params;
        strcpy(params.Param1, "\0");
        strcpy(params.Param2, "\0");
        strcpy(params.Param3, "\0");
        int len = generateMessage(1, 0, params, buffer);
        sendMessage(g_args, buffer, len);
}

void in_room_list(const int roomId, const char *roomName)
{
    printf("////////////////////////////////////// in rooms\n");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (g_rooms[i].roomId != -1)
        {
            g_rooms[i].roomId = roomId;
            strcpy(g_rooms[i].roomName, roomName);

            // Parameters params;
            // char buffer[BUFFER];
            // // online
            // strcpy(params.Param1, util_int_to_str(roomId));
            // int len = generateMessage(3, 0, params, buffer);
            // sendMessage(g_args, buffer, len);
            // printf("//////////////////////////////////////sent conv\n");
        }
    }
    printf("//////////////////////////////////////continue room\n");
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
    printf("//////////////////////////////////////in mem\n");
    for (int i = 0; i < 10000; i++)
    {
        if (g_room_member[i].memName[0] != '\0')
        {
            g_room_member[i].roomId = roomId;
            strcpy(g_room_member[i].memName, member);
        }
    }
printf("//////////////////////////////////////continue mem\n");        
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
    printf("//////////////////////////////////////in conv\n");

    for (int i = 0; i < 1024; i++)
    {
        if (g_message[i].timestamp[0] == '\0')
        {
            g_message[i].roomId = roomId;
            char cont[500];
            util_get_substring(messageStr, cont, 50, strlen(messageStr) - 50);
            strcpy(g_message[i].timestamp, messageStr);
            strcpy(g_message[i].userId, username);
            strcpy(g_message[i].content, cont);
            break;
        }
    }
    printf("//////////////////////////////////////continue conv\n");
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
    printf("//////////////////////////////////////in_login\n");
    // request initial data for running
    strcpy(g_username, username);
    Parameters params;
    char buffer[BUFFER];
    // online
    // strcpy(params.Param1, util_int_to_str(g_rtd));
    // strcpy(params.Param2, "\0");
    // strcpy(params.Param3, "\0");
    // int len = generateMessage(0, 0, params, buffer);
    // sendMessage(g_args, buffer, len);
    // printf("//////////////////////////////////////sent ping\n");
    // recvAndProcess(g_args);
    usleep(5000);
    // // friend
    // params.Param1[0]='\0';
    // len = generateMessage(1, 0, params, buffer);
    // sendMessage(g_args, buffer, len);
    // printf("//////////////////////////////////////sent friends\n");
    // usleep(5000);
    // //room
    // strcpy(params.Param1, username);
    // len = generateMessage(2, 0, params, buffer);
    // printf("//////////////////////////////////////sent room\n");
    // sendMessage(g_args, buffer, len);
}

void out_login(const char *username, const char *password)
{
    printf("//////////////////////////////////////out_login\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, username);
    strcpy(params.Param2, password);
    int len = generateMessage(0, 1, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_register(const char *username, const char *password)
{
    printf("//////////////////////////////////////out_register\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, username);
    strcpy(params.Param2, password);
    int len = generateMessage(0, 2, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_change_password(const char *username, const char *oldpass, const char *newpass)
{
    printf("//////////////////////////////////////out_pass\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, username);
    strcpy(params.Param2, oldpass);
    strcpy(params.Param3, newpass);
    int len = generateMessage(0, 3, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_get_friend_list()
{
    printf("//////////////////////////////////////out_friend\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, "\0");
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(1, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_sent_friend_request(const char *username, const char *friendname)
{
    printf("//////////////////////////////////////out_request\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, username);
    strcpy(params.Param2, friendname);
    strcpy(params.Param3, "\0");
    int len = generateMessage(1, 1, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_sent_friend_response(const char *username, const char *friendname)
{
    printf("//////////////////////////////////////out_response\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, username);
    strcpy(params.Param2, friendname);
    strcpy(params.Param3, "\0");
    int len = generateMessage(1, 2, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_get_room_list(const char *username)
{
    printf("//////////////////////////////////////out_room\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, username);
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(2, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_get_room_members(const int roomId)
{
    printf("//////////////////////////////////////out_member\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, util_int_to_str(roomId));
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(2, 1, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_create_room(const char *roomName, const char *username)
{
    printf("//////////////////////////////////////out_create\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, roomName);
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0");
    int len = generateMessage(2, 2, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_add_member(const int roomId, const char *username)
{
    printf("//////////////////////////////////////out_add_member\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, util_int_to_str(roomId));
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0");
    int len = generateMessage(2, 3, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_remove_member(const int roomId, const char *username)
{
    printf("//////////////////////////////////////out_remove_member\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, util_int_to_str(roomId));
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0");
    int len = generateMessage(2, 4, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_get_conversation(const int roomId)
{
    printf("//////////////////////////////////////out_conv\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, util_int_to_str(roomId));
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0");
    int len = generateMessage(3, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

void out_get_prev_conversation(const int roomId, const char *timestamp)
{
    printf("//////////////////////////////////////out_prev_conv\n");
    Parameters params;
    char buffer[BUFFER];
    strcpy(params.Param1, util_int_to_str(roomId));
    strcpy(params.Param2, timestamp);
    strcpy(params.Param3, "\0");
    int len = generateMessage(3, 0, params, buffer);
    sendMessage(g_args, buffer, len);
}

// int main(int argc, char *argv[]){
//     if (argc != 3) {
//         printf("Usage: %s ServerIPAddress PortNumber\n", argv[0]);
//         exit(1);
//     }
//     char *clientIP = argv[1];
//     int clientPort = atoi(argv[2]);

//     run_client(clientIP,clientPort);

//     //out_login("username", "password");
// }
/////////////////////////////////////////////////////////////////////////