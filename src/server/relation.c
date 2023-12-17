#include "feature.h"

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
int s_rela_friend_request (const char * username, const char * destination){

}
int s_rela_friend_response (const char * username, char * destination, const char response){

}