#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "utility.h"

typedef struct Parameters{
    char Param1[1024];
    char Param2[1024];
    char Param3[1024];
} Parameters;

uint32_t getProtocolOpcode (const char *message);
uint32_t getProtocolFunctionCode (const char *message);
void getProtocolPayload (const char *message, char * buffer, int size);
void getProtocolParameters(const char *payload, Parameters *parameters);
int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer);
void printCode(const char * binaryStr, size_t size);
#endif