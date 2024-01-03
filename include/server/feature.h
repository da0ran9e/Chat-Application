#ifndef FEATURE_H
#define FEATURE_H

#include "../database/dbc.h"
#include "connection.h"
#include "log.h"

int handle_features(const int user, int op, int func, const Parameters params);
int feat_online_list (const int clientSock, const int rtd);
int feat_login (const int clientSock, const char * username, const char * password);
int feat_register (const int clientSock, const char * username, const char * password);
int feat_change_password (const int clientSock, const char *username, const char * oldpass, const char * newpass);
int feat_friend_list (const int clientSock);
int feat_request_friend (int clientSock, const char * username, const char * destination);
int feat_response_request (int clientSock, const char * username, const char * destination, const char * response);
int feat_room_list (const int clientSock, const char * username);
int feat_room_members (const int clientSock, const int roomId);
int feat_room_create(const int clientSock, const char * name, const char * username);
int feat_add_member(const int clientSock, const int roomId, const char * username);
int feat_remove_member(const int clientSock, const int roomId, const char * username);
int feat_conversation (const int clientSock, const int roomId);
int feat_load_more(const int clientSock, const int roomId, const char * timestamp);
int feat_new_message(const int clientSock, const char * username, const int roomId, const char * message);

int writeMessage(const int op, const int func, const Parameters params, char * buffer);
int readMessage(const char * buffer, const int size, Parameters params);
void *handleClient(void *args);
void runServer(int serverSocket);
void _s_init (int port);

int s_auth_handle_login (const char * username, const char * password);
int s_auth_handle_register (const char * username, const char * password);
int s_auth_change_password (const char *username, const char * oldpass, const char * newpass);
int s_conv_get_conversation (const int roomId, const char *timestamp, char (* messageList)[50], int *count);
int s_conv_get_message(const int roomId, const char * timestamp, Message *message);
int s_conv_new_message (const char *username, const int roomId, const char *message);
int s_rela_friend_list (const char * username, char (* friendlist)[50], int *count);
int s_rela_addfriendship(const char * username, const char * destination, int *room);
int s_room_list (const char *username, Room * roomList, int *count);
int s_room_members (const int roomId, char (* peopleList)[50], int *count);
int s_room_create (const char *roomName, const char *adminUsername, int *roomId);
int s_room_add_member (const int roomId, const char * username);
int s_room_remove_member (const int roomId, const char * username);

#endif