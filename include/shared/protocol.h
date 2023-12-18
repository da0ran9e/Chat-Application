#ifndef PROTOCOL_H
#define PROTOCOL_H

typedef struct Parameters{
    char Param1[BUFFER];
    char Param2[BUFFER];
    char Param3[BUFFER];
} Parameters;

uint32_t getProtocolOpcode (const char *message);
uint32_t getProtocolFunctionCode (const char *message);
void getProtocolPayload (const char *message, char * buffer, int size);
Parameters getProtocolParameters(const char *payload, Parameters parameters);
int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer);

#endif