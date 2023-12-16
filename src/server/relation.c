#include "../../database/database.h"

int s_rela_friend_list (const char * username, char * friendlist, int count);
int s_rela_friend_request (const char * username, const char * destination);
int s_rela_friend_response (const char * username, char * destination, const char response);