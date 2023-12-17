#include "../../database/dbc.h"

int s_auth_handle_login (const char * username, const char * password);
int s_auth_handle_register (const char * username, const char * password);
int s_auth_change_password (const char *username, const char * oldpass, const char * newpass);

/*
* 210 mean login successfully
* 110 mean login failed
* 410 mean connection failed
*/
int s_auth_handle_login (const char * username, const char * password){
    switch (dbc_client_login(username, password))
    {
    case ACCEPT:
        return 210;
        break;
    case DENY:
        return 110;
        break;
    default:
        return 410;
        break;
    }
}

/*
* 220: register done
* 120: username already taken 
* 420: connection lost
*/
int s_auth_handle_register (const char * username, const char * password){
    switch (dbc_client_register(username, password))
    {
    case DONE:
        return 220;
        break;
    case CONFLICT:
        return 120;
        break;
    default:
        return 420;
        break;
    }
}

/*
* 230: change done
* 130: cannot change 
* 430: connection lost
*/
int s_auth_change_password (const char *username, const char * oldpass, const char * newpass){
    switch (dbc_change_password(username, oldpass, newpass))
        {
        case ACCEPT:
            return 230;
            break;
        case DENY:
            return 130;
            break;
        default:
            return 430;
            break;
        }
}