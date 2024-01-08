#include "../../include/database/dbc.h"

/*
* dbc_client_login
*
* [IN] param1: username[]
* [IN] param2: password[]
*
* [OUT]: enum {DENY/ACCEPT}
*/
enum DBStatus dbc_client_login (const char * username, const char * password){
    PGconn * conn = database_start ();

    int result = execute_login_query(conn, username, password);

    database_exit(conn);

    if(result == 0) return DENY;
    else return ACCEPT;
}

/*
* dbc_client_register
*
* [IN] param1: username[]
* [IN] param2: password[]
*
* [OUT]: enum {CONFLICT/DONE}
*/
enum DBStatus dbc_client_register (const char * username, const char * password){
    PGconn * conn = database_start ();
    
    int result = execute_register_query(conn, username, password);

    database_exit(conn);

    if(result == 0) return CONFLICT;
    else return DONE;
}

/*
* dbc_change_password
*
* [IN] param1: username[]
* [IN] param2: oldpassword[]
* [IN] param3: newpassword[]
*
* [OUT]: enum {DENY/ACCEPT}
*/
enum DBStatus dbc_change_password (const char * username, const char * oldpass, const char * newpass){
    PGconn * conn = database_start ();
    
    int result = execute_change_password_query(conn, username, oldpass, newpass);

    database_exit(conn);

    if(result == 0) return DENY;
    else return ACCEPT;
}

/*
* dbc_get_friendlist
*
* [IN] param1: username[]
* [IN] param2: friendList[][]
*
* [OUT]: number of rows
*/
int dbc_get_friendlist (const char * username, char (* friendList)[50]){
    PGconn * conn = database_start ();
    
    int result = execute_get_friend_list_query(conn, username, friendList);

    database_exit(conn);
    return result;
}
int dbc_get_requestlist (const char * username, char (* requestList)[50]){
    PGconn * conn = database_start ();
    
    int result = execute_get_request_list_query(conn, username, requestList);

    database_exit(conn);
    return result;
}
/*
* dbc_add_friend
*
* [IN] param1: username[]
* [IN] param2: friendname[]
*
* [OUT]: enum {DENY/ACCEPT}
*/
enum DBStatus dbc_request_friend (const char * username, const char * friendname){
    PGconn * conn = database_start ();
    
    int result = execute_send_friend_query(conn, username, friendname);

    database_exit(conn);

    if(result == 0) return DENY;
    else return ACCEPT;
}

/*
* dbc_add_friend
*
* [IN] param1: username[]
* [IN] param2: friendname[]
*
* [OUT]: enum {DENY/ACCEPT}
*/
enum DBStatus dbc_add_friend (const char * username, const char * friendname){
    PGconn * conn = database_start ();
    
    int result = execute_add_friend_query(conn, username, friendname);

    database_exit(conn);

    if(result == 0) return DENY;
    else return ACCEPT;
}

/*
* dbc_delete_friend
*
* [IN] param1: username1[]
* [IN] param2: username2[]
*
* [OUT]: enum {NOTFOUND/ACCEPT}
*/
enum DBStatus dbc_delete_friend (const char * username1, const char * username2){
    PGconn * conn = database_start ();
    
    int result = execute_delete_friend_query(conn, username1, username2);

    database_exit(conn);

    if(result == 0) return NOTFOUND;
    else return ACCEPT;
}

/*
* dbc_get_roomlist
*
* [IN] param1: username[]
* [IN] param2: roomList[] 
* struct Room {roomId, roomName[]}
*
* [OUT] number of rows
*/
int dbc_get_roomlist (const char * username, Room * roomList){
    PGconn * conn = database_start ();
    
    int result = execute_get_room_list_query(conn, username, roomList);

    database_exit(conn);

    return result;
}

/*
* dbc_get_members
*
* [IN] param1: roomId[]
* [IN] param2: memberList[][] 
*
* [OUT] number of rows
*/
int dbc_get_members (const int roomId, char (*memberList)[50]){
    PGconn * conn = database_start ();
    
    int result = execute_get_people_in_room_query(conn, roomId, memberList);

    database_exit(conn);

    return result;
}

/*
* dbc_create_private_room
*
* [IN] param1: username1[]
* [IN] param2: username2[]
*
* [OUT] roomId
*/
int dbc_create_private_room (const char * username1, const char * username2){
    PGconn * conn = database_start ();
    
    int roomId = execute_create_private_room_query(conn, username1, username2);

    database_exit(conn);

    return roomId;
}

/*
* dbc_create_room
*
* [IN] param1: roomname[]
* [IN] param2: username[] this will be room admin
*
* [OUT] roomId
*/
int dbc_create_room (const char * roomname, const char * username){
    PGconn * conn = database_start ();
    
    int roomId = execute_create_new_room_query(conn, roomname, username);

    database_exit(conn);

    return roomId;
}

/*
* dbc_add_member
*
* [IN] param1: username[]
* [IN] param2: roomId
*
* [OUT] enum {FULL/ACCEPT}
*/
enum DBStatus dbc_add_member (const char * username, const int roomId){
    PGconn * conn = database_start ();
    
    int result = execute_add_person_to_room_query(conn, username, roomId);

    database_exit(conn);

    if(result == 0) return FULL;
    else return ACCEPT;
}

/*
* dbc_remove_member
*
* [IN] param1: username[]
* [IN] param2: roomId
*
* [OUT] enum {ERROR/ACCEPT}
*/
enum DBStatus dbc_remove_member (const char * username, const int roomId){
    PGconn * conn = database_start ();
    
    int result = execute_remove_person_from_room_query(conn, username, roomId);
    
    database_exit(conn);

    if(result == 0) return ERROR;
    else return ACCEPT;
}

/*
* dbc_get_new_conversation
*
* [IN] param1: roomId
* [IN] param2: messageList[][]
*
* [OUT] number of rows
*/
int dbc_get_new_conversation (const int roomId, char (*messageList)[50]){
    PGconn * conn = database_start ();
    
    int result = execute_get_room_current_conversation_query(conn, roomId, messageList);
    
    database_exit(conn);

    return result;
}

/*
* dbc_get_prev_conversation
*
* [IN] param1: roomId
* [IN] param2: timestamp[]
* [IN] param3: messageList[][]
*
* [OUT] number of rows
*/
int dbc_get_prev_conversation (const int roomId, const char * timestamp, char (*messageList)[50]){
    PGconn * conn = database_start ();
    
    int result = execute_get_room_conversation_query(conn, roomId, timestamp, messageList);
    
    database_exit(conn);

    return result;
}

/*
* dbc_get_message
*
* [IN] param1: roomId
* [IN] param2: timestamp[]
* [IN] param3: message[] 
* Message{roomId,userId,timestamp[], content[]}
*
* [OUT] enum{ERROR/ACCEPT}
*/
enum DBStatus dbc_get_message (const int roomId, const char * timestamp, Message *message){
    PGconn * conn = database_start ();
    
    int result = execute_get_conversation_content_query(conn, roomId, timestamp, message);
   //printf("dbc res: %d\n",result);
    database_exit(conn);

    if(result == 0) return ERROR;
    else return ACCEPT;
}

/*
* dbc_new_message
*
* [IN] param1: username
* [IN] param2: roomId
* [IN] param3: message[] 
*
* [OUT] enum{ERROR/ACCEPT}
*/
enum DBStatus dbc_new_message (const char * username, const int roomId, const char * message){
    PGconn * conn = database_start ();
    
    int result = execute_add_message_to_conversation_query(conn, username, roomId, message);
    
    database_exit(conn);

    if(result == 0) return ERROR;
    else return ACCEPT;
}

// int main() {
//     return 0;
// }