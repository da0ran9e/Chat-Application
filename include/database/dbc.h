#ifndef DBC_H
#define DBC_H

#include "queries.h"

enum DBStatus   dbc_client_login        (char * username, char * password);
enum DBStatus   dbc_client_register     (char * username, char * password);
enum DBStatus   dbc_change_password     (char * username, char * oldpass, char * newpass);
int             dbc_get_friendlist      (char * username, char * friendList);
enum DBStatus   dbc_add_friend          (char * username, char * friendname);
enum DBStatus   dbc_delete_friend       (char * username1, char * username2);
int             dbc_get_roomlist        (char * username, Room * roomList);
int             dbc_get_members         (int roomId, char * memberList);
int             dbc_create_private_room (char * username1, char * username2);
int             dbc_create_room         (char * roomname, char * username);
enum DBStatus   dbc_add_member          (char * username, int roomId);
enum DBStatus   dbc_remove_member       (char * username, int roomId);
int             dbc_get_new_conversation(int roomId, char * messageList);
int             dbc_get_prev_conversation(int roomId, char * timestamp, char * messageList);
enum DBStatus   dbc_get_message         (int roomId, char * timestamp, Message message);
enum DBStatus   dbc_new_message         (char * username, int roomId, char * message);

#endif
