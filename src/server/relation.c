#include "../../include/server/feature.h"

/*
* 201: success
* 101: fail
*/
int s_rela_friend_list (const char * username, char * friendlist, int count){
    count = dbc_get_friendlist(username, friendlist);
    if (count > 0) return 202;
    else return 102;
}

/*
* 211: success
* 111: fail
* 411: connection lost
*/
int s_rela_addfriendship(const char * username, const char * destination, int room){
    switch (dbc_add_friend(username, destination))
    {
    case ACCEPT:
        room = dbc_create_private_room(username, destination);
        return 211;
        break;
    case DENY:
        return 111;
        break;
    default:
        return 411;
        break;
    }
}
