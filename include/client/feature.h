#ifndef FEATURE_H
#define FEATURE_H

#include "../shared/common.h"
#include "connection.h"

#define MAX_CLIENTS 100

typedef struct RoomMember{
    int roomId;
    char memName[50];
    memName[0] = '\0';
} RoomMember;

int g_socket = -1;
char g_address[15];
int g_port = -1;
int g_rtd = 9999;
char g_user[MAX_CLIENTS][50];
int g_rtds[MAX_CLIENTS];
char g_friend[MAX_CLIENTS][50];
Room g_rooms[MAX_CLIENTS];
Message g_message[BUFFER];
RoomMember g_room_member[10000];

int handle_send_message(enum RequestEvent request, const Parameters params, char * message);
int handle_receive_message(const char * messsge);
void in_online_list(const char * username, int rtd);
void in_friend_list(const char * username);
void in_room_list(const int roomId, const char * roomName);
void in_member_list(const char * member, const int roomId);
void in_conversation(const int roomId, const char * timestamp, const char * username);
void in_message(const int roomId, const char * timestamp, const char * message);
void in_login_done(const char * username);
void out_login(const char * username, const char * password);
void out_register(const char * username, const char * password);
void out_change_password(const char * username, const char * oldpass, const char * newpass);
void out_get_friend_list()
void out_sent_friend_request(const char * username, const char * friendname);
void out_sent_friend_response(const char * username, const char * friendname);
void out_get_room_list(const char * username);
void out_get_room_members(const int roomId);
void out_create_room(const char * roomName, const char * username);
void out_add_member(const int roomId, const char * username);
void out_remove_member(const int roomId, const char * username);
void out_get_conversation(const int roomId);


#endif