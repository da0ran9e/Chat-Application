#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "constants.h"
#include "protocol.h"

typedef struct Room{
    int roomId;
    char roomName[50];
}Room;

typedef struct Message{
    int roomId;
    int userId;
    char timestamp[20];
    char content[500];
}Message;
#endif