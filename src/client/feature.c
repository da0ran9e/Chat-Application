#include "../../include/client/feature.h"

int handle_send_message(enum RequestEvent request, const Parameters params, char * message){
    int op = request%10;
    int func = (request%100)\10;
    int len = generateMessage(op, func, params, message);

    send_message(g_socket, message, len);
}

int handle_receive_message(const char * messsge){
    int op = getProtocolOpcode(messsge);
    int func = getProtocolFunctionCode(messsge);
    int code = 10*func + op;
    int status;
    Parameters params;
    char payload[BUFFER-8];
    int plSize=0;
    getProtocolPayload(messsge, payload, plSize);
    getProtocolParameters(payload, &params);
    
    //handle
    switch (code)
    {
    case 00:
        in_online_list(params.Param1, atoi(params.Param2));
        status = 200;
        break;
    case 10:
        if (!strcmp(params.Param1, "error")){
            status = 310;
        }else{
            status = 210;
            in_login_done(params.Param1);
        }
        break;
    case 20:
        if (!strcmp(params.Param1, "error")){
            status = 320;
        }else{
            status = 220;
        }
        break;
    case 30:
        if (!strcmp(params.Param1, "error")){
            status = 330;
        }else{
            status = 230;
        }
        break;
    case 01:
        if (!strcmp(params.Param1, "error")){
            status = 301;
        }else{
            status = 201;
            in_friend_list(params.Param1);
        }
        break;
    case 11:
        if (!strcmp(params.Param1, "error")){
            status = 311;
        }else{
            status = 211;
        }
        break;
    case 21:
        if (!strcmp(params.Param1, "error")){
            status = 321;
        }else if (!strcmp(params.Param1, "accept")){
            status = 221;
        }
        else {
            status = 121;
        }
        break;
    case 02:
        if (!strcmp(params.Param1, "error")){
            status = 302;
        }else{
            status = 202;
            in_room_list(atoi(params.Param1), params.Param2);
        }
        break;
    case 12:
        if (!strcmp(params.Param1, "error")){
            status = 312;
        }else{
            status = 212;
            in_friend_list(params.Param1);
        }
        break;
    case 22:
        if (!strcmp(params.Param1, "error")){
            status = 322;
        }else{
            status = 222;
            in_room_list(atoi(params.Param1),"\0");
        }
        break;
    case 32:
        if (!strcmp(params.Param1, "error")){
            status = 332;
        }else{
            status = 232;
        }
        break;
    case 42:
        if (!strcmp(params.Param1, "error")){
            status = 342;
        }else{
            status = 242;
        }
        break;
    case 03:
        if (!strcmp(params.Param1, "error")){
            status = 303;
        }else{
            status = 203;
            in_conversation(atoi(params.Param1), params.Param2, params.Param3);
        }
        break;
    case 13:
        if (!strcmp(params.Param1, "error")){
            status = 313;
        }else{
            status = 213;
            in_message(atoi(params.Param1), params.Param2, params.Param3);
        }
        break;
    default:
        break;
    }
    return status;
}

void in_online_list(const char * username, int rtd){
    for(int i=0; i<MAX_CLIENTS; i++){
        if (g_user[i][0]!='\0') {
            strcpy(g_user[i], username);
            g_rtds[i] = rtd;
        }
    }
}

void in_friend_list(const char * username){
    for(int i=0; i<MAX_CLIENTS; i++){
        if (g_friend[i][0]!='\0') {
            strcpy(g_friend[i], username);
        }
    }
}

void in_room_list(const int roomId, const char * roomName){
    for (int i=0; i<MAX_CLIENTS; i++){
        if (g_rooms[i].roomId == -1 || g_rooms[i].roomName[0] == '\0'){
            g_rooms[i].roomId == roomId;
            strcpy(g_rooms[i].roomName, roomName);

            Parameters params;
            char buffer[BUFFER];
            // online 
            strcpy(params.Param1, util_int_to_str(roomId)); 
            int len = generateMessage(3, 0, p, buffer);
            send_message(g_socket, buffer, len);
        }
    }
}

void in_member_list(const char * member, const int roomId){
    for (int i=0; i<10000; i++){
        if (g_room_member[i].memName[0] != '\0'){
            g_room_member[i].roomId = roomId;
            strcpy(g_room_member[i], member);
        }
    }
}

void in_conversation(const int roomId, const char * timestamp, const char * username){
    for (int i=0; i<1024; i++){
        if (g_message[i].timestamp[0] == '\0'){
            g_message[i].roomId = roomId;
            strcpy(g_message[i].timestamp, timestamp);
            strcpy(g_message[i].userId, username);
        }
    }
}

void in_message(const int roomId, const char * timestamp, const char * message){
    for (int i=0; i<1024; i++){
        if (!strcmp(g_message[i].timestamp, timestamp) && roomId == g_message[i].roomId){
            strcpy(g_message[i].content, message);
        }
    }
}

void in_login_done(const char * username){
    // request initial data for running
    Parameters params;
    char buffer[BUFFER];
    // online 
    strcpy(params.Param1, util_int_to_str(g_rtd)); 
    int len = generateMessage(0, 0, p, buffer);
    send_message(g_socket, buffer, len);
    // friend 
    params.Param1[0]='\0';
    len = generateMessage(1, 0, params, buffer);
    send_message(g_socket, buffer, len);
    //room
    strcpy(params.Param1, username);    
    len = generateMessage(2, 0, params, buffer);
    send_message(g_socket, buffer, len);

}

void out_login(const char * username, const char * password){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, password); 
    int len = generateMessage(0, 1, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_register(const char * username, const char * password){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, password); 
    int len = generateMessage(0, 2, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_change_password(const char * username, const char * oldpass, const char * newpass){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, oldpass);
    strcpy(params.Param3, newpass); 
    int len = generateMessage(0, 3, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_friend_list(){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, "\0"); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(1, 0, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_sent_friend_request(const char * username, const char * friendname){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, friendname);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(1, 1, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_sent_friend_response(const char * username, const char * friendname){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, friendname);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(1, 2, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_room_list(const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, username); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 0, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_room_members(const int roomId){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, atoi(roomId)); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 1, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_create_room(const char * roomName, const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, roomName); 
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 2, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_add_member(const int roomId, const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, atoi(roomId)); 
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 3, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_remove_member(const int roomId, const char * username){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, atoi(roomId)); 
    strcpy(params.Param2, username);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(2, 4, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_conversation(const int roomId){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, stoi(roomId)); 
    strcpy(params.Param2, "\0");
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(3, 0, p, buffer);
    send_message(g_socket, buffer, len);
}

void out_get_prev_conversation(const int roomId, const char * timestamp){
    Parameters params;
    char buffer[BUFFER]; 
    strcpy(params.Param1, stoi(roomId)); 
    strcpy(params.Param2, timestamp);
    strcpy(params.Param3, "\0"); 
    int len = generateMessage(3, 0, p, buffer);
    send_message(g_socket, buffer, len);
}