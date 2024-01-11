#ifndef FEATURE_H
#define FEATURE_H

#include "../shared/common.h"
#include "../client/connection.h"

#define PING_INTERVAL 500 // Interval in seconds for sending ping messages

// Structure to pass arguments to the thread
struct ThreadArgs
{
    int clientSocket;
    pthread_mutex_t threadMutex;
};

typedef struct RoomMember{
    int roomId;
    char memName[50];
    //memName[0] = '\0';
} RoomMember;

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

int handle_receive_message(const char *message, int len);
void in_online_list(const char * username, const int rtd);
void in_friend_list(const char * username);
void in_request_list(const char * username);
void in_room_list(const int roomId, const char * roomName);
void in_member_list(const char * member, const int roomId);
void in_conversation(const int roomId, const char * timestamp, const char * username);
void in_message(const int roomId, const char * timestamp, const char * message);
void in_login_done(const char * username);
void showFeatures();
void c_online();
void c_login();
void c_register();
void c_change_pass();
void c_friend_list();
void c_request_list();
void c_send_friend_request();
void c_response_friend_request();
void c_rooms();
void c_members();
void c_create();
void c_add_member();
void c_remove_member();
void c_conversation();
void c_chat();
void c_load_all();

void sendMessage(void *args, const char *buffer, int size);
int recvAndProcess(void *args);
void *sendThread(void *args);
void *sendPingMessages(void *args);
void run_client(const char *clientIP, const int clientPort);

#endif