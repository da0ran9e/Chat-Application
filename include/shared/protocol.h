#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "utility.h"

enum RequestEvent {
    REQ_PING            = 00,
    REQ_LOGIN           = 10,
    REQ_REGISTER        = 20,
    REQ_CHANGE_PASS     = 30,
    REQ_FRIEND_LIST     = 01,
    REQ_FRIEND_REQUEST  = 11,
    REQ_RESPONSE_REQUEST= 21,
    REQ_ROOM_LIST       = 02,
    REQ_ROOM_MEMBER     = 12,
    REQ_CREATE_ROOM     = 22,
    REQ_ADD_MEMBER      = 32,
    REQ_REMOVE_MEMBER   = 42,
    REQ_CONVERSATION    = 03,
    REQ_MESSAGE         = 13,
};

typedef struct Parameters{
    char Param1[1024];
    char Param2[1024];
    char Param3[1024];
} Parameters;

uint32_t getProtocolOpcode (const char *message);
uint32_t getProtocolFunctionCode (const char *message);
void getProtocolPayload (const char *message, char * buffer, int size);
void getProtocolParameters(const char *payload, Parameters parameters);
int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer);

#endif