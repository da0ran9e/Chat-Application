#include "../../database/dbc.h"
#include "../../include/shared/common.h"

int s_room_list (const char *username, Room * roomList, int count);
int s_room_members (const int roomId, char * peopleList, int count);
int s_room_create (const char *roomName, const char *adminUsername, int roomId);

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