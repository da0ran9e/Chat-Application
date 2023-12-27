#include "../../include/server/feature.h"

/*
* 203: success
* 103: fail
*/
int s_conv_get_conversation (const int roomId, const char *timestamp, char (* messageList)[50], int *count){
    if (timestamp == NULL){
        *count = dbc_get_new_conversation(roomId, messageList);
    }
    else{
        *count = dbc_get_prev_conversation(roomId, timestamp, messageList);
    }
    if (count > 0) return 203;
    else return 103;    
}

int s_conv_get_message(const int roomId, const char * timestamp, Message *message){
    switch(dbc_get_message(roomId, timestamp, message)){
        case ACCEPT: 
            return 203;
            break;
        case ERROR:
            return 103;
            break;
        default:
            return 403;
            break;
    }
}

/*
* 213: success
* 113: fail
* 413: connection lost
*/
int s_conv_new_message (const char *username, const int roomId, const char *message){
    switch (dbc_new_message(username, roomId, message))
    {
    case ACCEPT:
        return 213;
        break;
    case ERROR:
        return 113;
        break;
    default:
        return 413;
        break;
    }
}