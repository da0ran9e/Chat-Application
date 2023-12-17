#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "constants.h"
#include "utility.h"
#include "protocol.h"

typedef struct Parameters{
    char Param1[BUFFER];
    char Param2[BUFFER];
    char Param3[BUFFER];
} Parameters;

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