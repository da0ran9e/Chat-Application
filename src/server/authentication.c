#include "../../database/database.h"

int s_auth_handle_login (const char * username, const char * password);
int s_auth_handle_register (const char * username, const char * password);
int s_auth_change_password (const char *username, const char * oldpass, const char * newpass);