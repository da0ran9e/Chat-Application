//#include "../../include/database.h"

#include <stdio.h>
#include <stdlib.h>

#include "libpq-fe.h"

// Function to convert an integer to a string
char* int_to_str(int value) {
    char* str = (char*)malloc(15);  // a maximum of 12 digits for an int
    snprintf(str, 15, "%d", value);
    return str;
}

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

void execute_get_friend_list_query(PGconn *conn, const char *username, char *friendlist) {
    const char *query = "SELECT * FROM get_friend_list($1)";
    const char *paramValues[1] = {username};
    char friends[100][50];

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    printf("Friend List for %s:\n", username);
    for (int i = 0; i < rows; ++i) {
        strcpy(friend[i], PQgetvalue(result, i, 0))
    }

    // Free the result
    PQclear(result);
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

void execute_delete_friend_query(PGconn *conn, const char *username1, const char *username2) {
    const char *query = "SELECT delete_friend($1, $2) AS delete_friend_status";
    const char *paramValues[2] = {username1, username2};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int deleteFriendStatus = atoi(PQgetvalue(result, 0, 0));
    printf("Delete Friend Status: %d\n", deleteFriendStatus);

    // Free the result
    PQclear(result);
}

void execute_get_room_list_query(PGconn *conn, const char *username) {
    const char *query = "SELECT * FROM get_room_list($1)";
    const char *paramValues[1] = {username};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    printf("Room List for %s:\n", username);
    for (int i = 0; i < rows; ++i) {
        printf("%s\n", PQgetvalue(result, i, 0));
    }

    // Free the result
    PQclear(result);
}

void execute_get_people_in_room_query(PGconn *conn, int room_id) {
    const char *query = "SELECT * FROM get_people_in_room($1)";
    char* room_id_str = int_to_str(room_id);
    const char *paramValues[1] = {room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    printf("People in Room %d:\n", room_id);
    for (int i = 0; i < rows; ++i) {
        printf("%s\n", PQgetvalue(result, i, 0));
    }

    // Free the result
    PQclear(result);
}


void execute_create_private_room_query(PGconn *conn, const char *username1, const char *username2) {
    const char *query = "SELECT create_private_room($1, $2) AS new_room_id";
    const char *paramValues[2] = {username1, username2};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int newRoomId = atoi(PQgetvalue(result, 0, 0));
    printf("New Private Room ID: %d\n", newRoomId);

    // Free the result
    PQclear(result);
}


//create group chat with admin 
void execute_create_new_room_query(PGconn *conn, const char *roomName, const char *adminUsername) {
    const char *query = "SELECT create_new_room($1, $2) AS new_room_id";
    const char *paramValues[2] = {roomName, adminUsername};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int newRoomId = atoi(PQgetvalue(result, 0, 0));
    printf("New Room ID: %d\n", newRoomId);

    // Free the result
    PQclear(result);
}

void execute_add_person_to_room_query(PGconn *conn, const char *username, int room_id) {
    const char *query = "SELECT add_person_to_room($1, $2) AS add_person_status";
    char* room_id_str = int_to_str(room_id);
    const char *paramValues[2] = {username, room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int addPersonStatus = atoi(PQgetvalue(result, 0, 0));
    printf("Add Person to Room Status: %d\n", addPersonStatus);

    // Free the result
    PQclear(result);
}

void execute_remove_person_from_room_query(PGconn *conn, const char *username, int room_id) {
    const char *query = "SELECT remove_person_from_room($1, $2) AS remove_person_status";
    char* room_id_str = int_to_str(room_id);
    const char *paramValues[2] = {username, room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int removePersonStatus = atoi(PQgetvalue(result, 0, 0));
    printf("Remove Person from Room Status: %d\n", removePersonStatus);

    // Free the result
    PQclear(result);
}

void execute_get_room_current_conversation_query(PGconn *conn, int room_id) {
    const char *query = "SELECT * FROM get_room_conversation($1, CURRENT_TIMESTAMP::TIMESTAMP)";
    char* room_id_str = int_to_str(room_id);
    const char *paramValues[1] = {room_id_str};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 1, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    printf("Room Conversation for Room %d at current timestamp:\n", room_id);
    for (int i = 0; i < rows; ++i) {
        printf("%s\n", PQgetvalue(result, i, 0));
    }

    // Free the result
    PQclear(result);

    // Free the room_id_str after using it
    free(room_id_str);
}



// Get Room Conversation before any time
void execute_get_room_conversation_query(PGconn *conn, int room_id, char *timestamp) {
    const char *query = "SELECT * FROM get_room_conversation($1, $2)";
    char* room_id_str = int_to_str(room_id);
    const char *paramValues[2] = {room_id_str, timestamp};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    printf("Room Conversation for Room %d before %s:\n", room_id, timestamp);
    for (int i = 0; i < rows; ++i) {
        printf("%s\n", PQgetvalue(result, i, 0));
    }

    // Free the result
    PQclear(result);
}

void execute_get_conversation_content_query(PGconn *conn, int room_id, const char *timestamp) {
    const char *query = "SELECT * FROM get_conversation_content($1, $2)";
    char* room_id_str = int_to_str(room_id);
    const char *paramValues[2] = {room_id_str, timestamp};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 2, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int rows = PQntuples(result);
    printf("Conversation Content for Room %d at %s:\n", room_id, timestamp);
    for (int i = 0; i < rows; ++i) {
        printf("UserID: %s, Message: %s\n", PQgetvalue(result, i, 0), PQgetvalue(result, i, 1));
    }

    // Free the result
    PQclear(result);
}

void execute_add_message_to_conversation_query(PGconn *conn, const char *username, int room_id, const char *message) {
    const char *query = "SELECT add_message_to_conversation($1, $2, $3) AS add_message_status";
    char* room_id_str = int_to_str(room_id);
    const char *paramValues[3] = {username, room_id_str, message};

    // Execute the query
    PGresult *result = PQexecParams(conn, query, 3, NULL, paramValues, NULL, NULL, 0);

    // Check the result
    check_result(result, conn);

    // Process the result
    int addMessageStatus = atoi(PQgetvalue(result, 0, 0));
    printf("Add Message to Conversation Status: %d\n", addMessageStatus);

    // Free the result
    PQclear(result);
}


int main() {
    PGconn *conn;

    // Establish a connection to the database
    conn = PQconnectdb("dbname=chat user=postgres password=anquynh123 host=localhost");

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }

    // Login query
    execute_login_query(conn, "user1", "password1");

    // Register a user
    execute_register_query(conn, "user4", "password4");

    // Change password query
    execute_change_password_query(conn, "user5", "password5", "new_password");

    // Get friendlist of a user
    execute_get_friend_list_query(conn, "user1");

    // Add a friend 
    execute_add_friend_query(conn, "user2", "user5");

    // Delete a friend 
    execute_delete_friend_query(conn, "user5", "user2");

    // Get Room List
    execute_get_room_list_query(conn, "user2");

    // Get the list of people in a room
    execute_get_people_in_room_query(conn, 1);

    // Create private room
    execute_create_private_room_query(conn, "user4", "user5");

    // Create new room (for group chat)
    execute_create_new_room_query(conn, "Room E", "user3");

    // Add a person to the room
    execute_add_person_to_room_query(conn, "user4", 3);

    // Remove a person from the room
    execute_remove_person_from_room_query(conn, "user4", 3);

    // Get conversation of a room with the current timestamp
    execute_get_room_current_conversation_query(conn, 1);

    // Get conversation of a room
    execute_get_room_conversation_query(conn, 1, "2023-12-01 12:00:00");

    // Get conversation content
    execute_get_conversation_content_query(conn, 1, "2023-12-01 12:00:00");

    // Add a message to a conversation
    execute_add_message_to_conversation_query(conn, "user3", 2, "Hello, My name is 3!");

    // 


    // Close the connection
    PQfinish(conn);

    return 0;
}