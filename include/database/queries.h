#ifndef QUERIES_H
#define QUERIES_H

#include <stdio.h>
#include <stdlib.h>

#include <libpq-fe.h>
#include "../shared/common.h"

int execute_login_query(PGconn *conn, const char *username, const char *password);
int execute_register_query(PGconn *conn, const char *username, const char *password);
int execute_change_password_query(PGconn *conn, const char *username, const char *oldPassword, const char *newPassword);
int execute_add_friend_query(PGconn *conn, const char *username1, const char *username2);
int execute_delete_friend_query(PGconn *conn, const char *username1, const char *username2);
int execute_create_private_room_query(PGconn *conn, const char *username1, const char *username2);
int execute_add_person_to_room_query(PGconn *conn, const char *username, int room_id);
int execute_remove_person_from_room_query(PGconn *conn, const char *username, int room_id);
int execute_get_friend_list_query(PGconn *conn, const char *username, char *friendlist);
int execute_get_room_list_query(PGconn *conn, const char *username, Room * roomlist);
int execute_get_people_in_room_query(PGconn *conn, int room_id, char * peoplelist);
int execute_create_new_room_query(PGconn *conn, const char *roomName, const char *adminUsername);
int execute_get_room_current_conversation_query(PGconn *conn, int room_id, char * messageList);
int execute_get_room_conversation_query(PGconn *conn, int room_id, char *timestamp);
int execute_get_conversation_content_query(PGconn *conn, int room_id, const char *timestamp, Message message);
int execute_add_message_to_conversation_query(PGconn *conn, const char *username, int room_id, const char *message);

#endif