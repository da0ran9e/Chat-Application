#include "../../include/database/queries.h"

void check_result(PGresult *result, PGconn *conn) {
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "Query execution failed: %s", PQerrorMessage(conn));
        PQclear(result);
        PQfinish(conn);
        exit(1);
    }
}

int execute_login_query(PGconn *conn, const char *username, const char *password) {
    const char *query = "SELECT login_user($1, $2) AS is_authorized";
    const char *paramValues[2] = {username, password};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int isAuthorized = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return isAuthorized;
}

int execute_register_query(PGconn *conn, const char *username, const char *password) {
    const char *query = "SELECT register_user($1, $2) AS registration_status";
    const char *paramValues[2] = {username, password};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int registrationStatus = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return registrationStatus;
}

int execute_change_password_query(PGconn *conn, const char *username, const char *oldPassword, const char *newPassword) {
    const char *query = "SELECT change_password($1, $2, $3) AS password_change_status";
    const char *paramValues[3] = {username, oldPassword, newPassword};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 3, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int passwordChangeStatus = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return passwordChangeStatus;
}

int execute_get_friend_list_query(PGconn *conn, const char *username, char (*friendlist)[50]) {
    const char *query = "SELECT * FROM get_friend_list($1)";
    const char *paramValues[1] = {username};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    for (int i = 0; i < rows; ++i) {
        strcpy(friendlist[i], PQgetvalue(result, i, 0));
    }

    // Free the result
    PQclear(result);

    return rows;
}

int execute_add_friend_query(PGconn *conn, const char *username1, const char *username2) {
    const char *query = "SELECT add_friend($1, $2) AS add_friend_status";
    const char *paramValues[2] = {username1, username2};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int addFriendStatus = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return addFriendStatus;
}

int execute_delete_friend_query(PGconn *conn, const char *username1, const char *username2) {
    const char *query = "SELECT delete_friend($1, $2) AS delete_friend_status";
    const char *paramValues[2] = {username1, username2};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int deleteFriendStatus = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return deleteFriendStatus;
}

int execute_get_room_list_query(PGconn *conn, const char *username, Room * roomlist) {
    const char *query = "SELECT * FROM get_room_list($1)";
    const char *paramValues[1] = {username};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    for (int i = 0; i < rows; ++i) {
        roomlist[i].roomId = atoi(PQgetvalue(result, i, 0));
        //printf("query room id: %d\n", roomlist[i].roomId);
        strcpy(roomlist[i].roomName, PQgetvalue(result, i, 1));
    }

    // Free the result
    PQclear(result);

    return rows;
}

int execute_get_people_in_room_query(PGconn *conn, const int room_id, char (* peoplelist)[50]) {
    const char *query = "SELECT * FROM get_people_in_room($1)";
    char* room_id_str = util_int_to_str(room_id);
    const char *paramValues[1] = {room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    for (int i = 0; i < rows; ++i) {
        strcpy(peoplelist[i], PQgetvalue(result, i, 0));
    }

    // Free the result
    PQclear(result);

    return rows;
}

int execute_create_private_room_query(PGconn *conn, const char *username1, const char *username2) {
    const char *query = "SELECT create_private_room($1, $2) AS new_room_id";
    const char *paramValues[2] = {username1, username2};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int newRoomId = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return newRoomId;
}

//create group chat with admin 
int execute_create_new_room_query(PGconn *conn, const char *roomName, const char *adminUsername) {
    const char *query = "SELECT create_new_room($1, $2) AS new_room_id";
    const char *paramValues[2] = {roomName, adminUsername};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int newRoomId = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return newRoomId;
}

int execute_add_person_to_room_query(PGconn *conn, const char *username, const int room_id) {
    const char *query = "SELECT add_person_to_room($1, $2) AS add_person_status";
    char* room_id_str = util_int_to_str(room_id);
    const char *paramValues[2] = {username, room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int addPersonStatus = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return addPersonStatus;
}

int execute_remove_person_from_room_query(PGconn *conn, const char *username, const int room_id) {
    const char *query = "SELECT remove_person_from_room($1, $2) AS remove_person_status";
    char* room_id_str = util_int_to_str(room_id);
    const char *paramValues[2] = {username, room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int removePersonStatus = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return removePersonStatus;
}

int execute_get_room_current_conversation_query(PGconn *conn, const int room_id, char (*messageList)[50]) {
    const char *query = "SELECT * FROM get_room_conversation($1, CURRENT_TIMESTAMP::TIMESTAMP)";
    char* room_id_str = util_int_to_str(room_id);
    const char *paramValues[1] = {room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    printf("Room Conversation for Room %d at current timestamp:\n", room_id);
    //printf("rows: %d\n", rows);
    for (int i = 0; i < rows; ++i) {
        strcpy(messageList[i], PQgetvalue(result, i, 0));
        //printf("mess %d: %s\n",i,messageList[i]);
    }

    // Free the result
    PQclear(result);

    // Free the room_id_str after using it
    free(room_id_str);

    return rows;
}

// Get Room Conversation before any time
int execute_get_room_conversation_query(PGconn *conn, const int room_id, const char *timestamp, char (* messageList)[50]) {
    const char *query = "SELECT * FROM get_room_conversation($1, $2)";
    char* room_id_str = util_int_to_str(room_id);
    const char *paramValues[2] = {room_id_str, timestamp};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);

    for (int i = 0; i < rows; ++i) {
        strcpy(messageList[i], PQgetvalue(result, i, 0));
    }

    // Free the result
    PQclear(result);

    return rows;
}

int execute_get_conversation_content_query(PGconn *conn, const int room_id, const char *timestamp, Message *message) {
    const char *query = "SELECT * FROM get_conversation_content($1, $2)";
    char* room_id_str = util_int_to_str(room_id);
    const char *paramValues[2] = {room_id_str, timestamp};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    if (PQntuples(result)<=0) return 0;

    // Process the result
    //printf("time: %s\n",timestamp);
    message->roomId = room_id;
    printf("mess room: %d\n", message->roomId);
    strcpy(message->userId, PQgetvalue(result, 0, 0));
    printf("username: %s\n", message->userId);
    strcpy(message->timestamp, timestamp);
    printf("time: %s\n", message->timestamp);
    strcpy(message->content, PQgetvalue(result, 0, 1));
    printf("chat: %s\n", message->content);

    // Free the result
    PQclear(result);

    return 1;
}

int execute_add_message_to_conversation_query(PGconn *conn, const char *username, const int room_id, const char *message) {
    const char *query = "SELECT add_message_to_conversation($1, $2, $3) AS add_message_status";
    char* room_id_str = util_int_to_str(room_id);
    const char *paramValues[3] = {username, room_id_str, message};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 3, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int addMessageStatus = atoi(PQgetvalue(result, 0, 0));

    // Free the result
    PQclear(result);

    return addMessageStatus;
}