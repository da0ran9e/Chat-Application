#ifndef COMMON_H
#define COMMON_H

#include <unistd.h>
#include "constants.h"
#include "protocol.h"

typedef struct Room{
    int roomId;
    char roomName[50];
}Room;

typedef struct Message{
    int roomId;
    char userId[50];//username
    char timestamp[50];
    char content[500];
}Message;
#endif