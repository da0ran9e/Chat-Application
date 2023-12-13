#include "db_connection.h"
#include "database.h"
#include "queries.h"

enum DBStatus client_login (char * username, char * password){
    PGconn * conn = database_start ();

    int result = execute_login_query(conn, username, password);
    if(result == 0) return DENY;
    else return ACCEPT;

    database_exit(conn);
}

enum DBStatus client_register (char * username, char * password){
    PGconn * conn = database_start ();
    
    int result = execute_register_query(conn, username, password);
    if(result == 0) return CONFLICT;
    else return DONE;

    database_exit(conn);
}

enum DBStatus change_password (char * username, char * oldpass, char * newpass){
    PGconn * conn = database_start ();
    
    int result = execute_change_password_query(conn, username, oldpass, newpass);
    if(result == 0) return DENY;
    else return ACCEPT;

    database_exit(conn);
}

enum DBStatus change_password (char * username, char * oldpass, char * newpass){
    PGconn * conn = database_start ();
    
    int result = execute_get_friend_list_query(conn, username, oldpass, newpass);
    if(result == 0) return DENY;
    else return ACCEPT;

    database_exit(conn);
}

void execute_get_friend_list_query(PGconn *conn, const char *username, char *friendlist);
int execute_add_friend_query(PGconn *conn, const char *username1, const char *username2);
int execute_delete_friend_query(PGconn *conn, const char *username1, const char *username2);
void execute_get_room_list_query(PGconn *conn, const char *username, char *roomlist, Room * roomlist);
void execute_get_people_in_room_query(PGconn *conn, int room_id, char * peoplelist);
int execute_create_private_room_query(PGconn *conn, const char *username1, const char *username2);
void execute_create_new_room_query(PGconn *conn, const char *roomName, const char *adminUsername);
int execute_add_person_to_room_query(PGconn *conn, const char *username, int room_id);
int execute_remove_person_from_room_query(PGconn *conn, const char *username, int room_id);
void execute_get_room_current_conversation_query(PGconn *conn, int room_id, char * messageList);
void execute_get_room_conversation_query(PGconn *conn, int room_id, char *timestamp);
void execute_get_conversation_content_query(PGconn *conn, int room_id, const char *timestamp, Message message);
void execute_add_message_to_conversation_query(PGconn *conn, const char *username, int room_id, const char *message);


int main() {
    return 0;
}