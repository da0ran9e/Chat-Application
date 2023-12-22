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
    getProtocolParameters(payload, params);
    
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
            in_login_done(params.Param2);
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
    // online 
    // friend 
    // conversation
    // message

}

