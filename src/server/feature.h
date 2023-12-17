#ifndef FEATURE_H
#define FEATURE_H

#include "../../database/dbc.h"

int s_auth_handle_login (const char * username, const char * password);
int s_auth_handle_register (const char * username, const char * password);
int s_auth_change_password (const char *username, const char * oldpass, const char * newpass);

int s_conv_get_conversation (const int roomId, const char *timestamp, char * messageList, int count);
int s_conv_new_message (const char *username, const int roomId, const char *message);

int s_rela_friend_list (const char * username, char * friendlist, int count);
int s_rela_friend_request (const char * username, const char * destination);
int s_rela_friend_response (const char * username, char * destination, const char response);

#endif