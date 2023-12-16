#include "../../database/database.h"

int s_conv_get_conversation (const int room_id, const char *timestamp);
int s_conv_new_message (const char *username, const int room_id, const char *message);