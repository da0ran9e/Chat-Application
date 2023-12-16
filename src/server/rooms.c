#include "../../database/database.h"
#include "../../include/shared/common.h"

int s_room_list (const char *username, Room * roomList);
int s_room_members (const int roomId, char * peopleList);
int s_room_create (const char *roomName, const char *adminUsername, int roomId);