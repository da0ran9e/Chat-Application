#include "../../include/client/feature.h"

int c_auth_login(const char * username, const char * password){
    Parameters p;
    char buffer[BUFFER];
    strcpy(p.Param1, username);
    strcpy(p.Param2, password);
    int len = generateMessage(0, 1, p, buffer);
    return send_message(g_socket, buffer, len);
}

void c_auth_register(const char * username, const char * password){
    Parameters p;
    char buffer[BUFFER];
    strcpy(p.Param1, username);
    strcpy(p.Param2, password);
    int len = generateMessage(0, 2, p, buffer);
    return send_message(g_socket, buffer, len);
}

void c_auth_change_password(const char * username, const char * oldpass, const char * newpass){
    Parameters p;
    char buffer[BUFFER];
    strcpy(p.Param1, username);
    strcpy(p.Param2, oldpass);
    strcpy(p.Param3, newpass);
    int len = generateMessage(0, 3, p, buffer);
    return send_message(g_socket, buffer, len);
}