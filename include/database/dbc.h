#ifndef DBC_H
#define DBC_H

#include "queries.h"

enum DBStatus   dbc_client_login        (const char * username, const char * password);
enum DBStatus   dbc_client_register     (const char * username, const char * password);
enum DBStatus   dbc_change_password     (const char * username, const char * oldpass, const char * newpass);
int             dbc_get_friendlist      (const char * username, char (* friendList)[50]);
enum DBStatus   dbc_add_friend          (const char * username, const char * friendname);
enum DBStatus   dbc_delete_friend       (const char * username1, const char * username2);
int             dbc_get_roomlist        (const char * username, Room * roomList);
int             dbc_get_members         (const int roomId, char (*memberList)[50]);
int             dbc_create_private_room (const char * username1, const char * username2);
int             dbc_create_room         (const char * roomname, const char * username);
enum DBStatus   dbc_add_member          (const char * username, const int roomId);
enum DBStatus   dbc_remove_member       (const char * username, const int roomId);
int             dbc_get_new_conversation(const int roomId, char (*messageList)[50]);
int             dbc_get_prev_conversation(const int roomId, const char * timestamp, char (*messageList)[50]);
enum DBStatus   dbc_get_message         (const int roomId, const char * timestamp, Message *message);
enum DBStatus   dbc_new_message         (const char * username, const int roomId, const char * message);

#endif
