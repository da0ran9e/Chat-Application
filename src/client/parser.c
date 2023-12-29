#include "../../include/client/feature.h"

void online_user_list_parser (char * jsonStr) {
    strcat(jsonStr, "[\n");
    for (int i=0; i<MAX_CLIENTS; i++){
        if(g_user[i][0] != '\0'){
            strcat(jsonStr, "\t{name: \"");
            strcat(jsonStr, g_user[i]);
            strcat(jsonStr, "\", friend: ");
            
            for (int j=0; j<MAX_CLIENTS; j++){
                if(!strcmp(g_user[i], g_friend[j])){
                    strcat(jsonStr, "true},\n");
                    break;
                }
                strcat(jsonStr, "false},\n");
            }
        }
    }
    strcat(jsonStr, "]");
    printf("%s\n", jsonStr);
}
void friend_list_parser (char * jsonStr){
    strcat(jsonStr, "[\n");
    for (int i=0; i<MAX_CLIENTS; i++){
        if(g_user[i][0] != '\0'){
            strcat(jsonStr, "\t{name: \"");
            strcat(jsonStr, g_user[i]);
            strcat(jsonStr, "\"},\n");
        }
    }
    strcat(jsonStr, "]");
    printf("%s\n", jsonStr);
}
void chat_room_list_parser (char * jsonStr){
    strcat(jsonStr, "[\n");
    for (int i=0; i<MAX_CLIENTS; i++){
        if(g_rooms[i].roomId!=-1){
            strcat(jsonStr, "\t{name: \"");
            strcat(jsonStr, g_rooms[i].roomName);
            strcat(jsonStr, "\",\n\tmember: [\n");
            for (int j=0; j<10000; j++){
                if(g_room_member[j].roomId == g_rooms[i].roomId){
                    strcat(jsonStr, "\t\t{membername: \"");
                    strcat(jsonStr, g_room_member[j].memName);
                    strcat(jsonStr, "\"},\n");
                }
            }
            strcat(jsonStr, "],\n\tmessages: [\n");
            for (int j=0; j<MAX_CLIENTS; j++){
                if(g_message[j].roomId == g_rooms[i].roomId){
                    strcat(jsonStr, "\t\t{\n\t\t\tuser: \"");
                    strcat(jsonStr, g_message[j].userId);
                    strcat(jsonStr, "\",\n\t\t\ttimestamp: \"");
                    strcat(jsonStr, g_message[j].timestamp);
                    strcat(jsonStr, "\",\n\t\t\tcontent: \"");
                    strcat(jsonStr, g_message[j].content);
                    strcat(jsonStr, "\"\n\t\t},\n")
                }
            }
            strcat(jsonStr, "\t]\n},\n")
        }
    }
    strcat(jsonStr, "]");
    printf("%s\n", jsonStr);
}

void getData(char * jsonStr){
    strcat(jsonStr, "{\n\tport: ");
    strcat(jsonStr, util_int_to_str(g_port));
    strcat(jsonStr, ",\n\trtd: ");
    strcat(jsonStr, util_int_to_str(g_rtd));
    strcat(jsonStr, ",\n\tonlineUsers: ");
    online_user_list_parser(jsonStr);
    strcat(jsonStr, ",\n\tfriends: ");
    friend_list_parser(jsonStr);
    strcat(jsonStr, ",\n\tchatRooms: ");
    chat_room_list_parser(jsonStr);
    strcat(jsonStr, "\n},");
    printf("%s", jsonStr);
}