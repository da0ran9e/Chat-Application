#include "../../include/server/feature.h"

/*
* 202: success
* 102: fail
*/
int s_room_list (const char *username, Room * roomList, int count){
    count = dbc_get_roomlist(username, roomList);
    if (count > 0) return 202;
    else return 102;
}

/*
* 212: success
* 112: fail
*/
int s_room_members (const int roomId, char * peopleList, int count){
    count = dbc_get_members(roomId, peopleList);
    if (count > 0) return 212;
    else return 112;
}
/*
* 222: success
* 122: fail
*/
int s_room_create (const char *roomName, const char *adminUsername, int roomId){
    roomId = dbc_create_room(roomName, adminUsername);
    if (count >= 0) return 222;
    else return 122;
}

/*
* 232: success
* 132: maximum member reached
*/
int s_room_add_member (const int roomId, const char * username){
    switch (dbc_add_member(username, roomId))
    {
    case ACCEPT:
        return 232;
        break;
    case FULL:
        return 132;
        break;
    default:
        return 432;
        break;
    }
}

/*
* 242: success
* 142: maximum member reached
*/
int s_room_remove_member (const int roomId, const char * username){
    switch (dbc_remove_member(username, roomId))
    {
    case ACCEPT:
        return 242;
        break;
    case ERROR:
        return 142;
        break;
    default:
        return 442;
        break;
    }
}