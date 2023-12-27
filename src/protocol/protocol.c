#include "../../include/shared/protocol.h"

uint32_t getProtocolOpcode (const char *message){
    // Extract the integer from the first 4 bytes
    uint32_t opCode;
    memcpy(&opCode, message, sizeof(opCode));
    return opCode;
}

uint32_t getProtocolFunctionCode (const char *message){
    // Extract the integer from the next 4 bytes
    uint32_t func;
    memcpy(&func, message + sizeof(uint32_t), sizeof(func));
    return func;
}

void getProtocolPayload (const char *message, char * buffer, int size){
    // Extract the payload from the entire of the message
    memcpy(buffer, message + 2*sizeof(uint32_t), size);
}

void getProtocolParameters(const char *payload, Parameters *parameters) {
    // Extract parameters from the payload
    uint32_t paramLen1, paramLen2, paramLen3;
    memcpy(&paramLen1, payload, sizeof(paramLen1));
    memcpy(&paramLen2, payload + sizeof(uint32_t) + paramLen1, sizeof(paramLen2));
    memcpy(&paramLen3, payload + 2 * sizeof(uint32_t) + paramLen1 + paramLen2, sizeof(paramLen3));

    // printf("paramlen1: %d\n", paramLen1);
    // printf("paramlen2: %d\n", paramLen2);
    // printf("paramlen3: %d\n", paramLen3);

    util_get_substring(payload, parameters->Param1, 4, paramLen1);
    util_get_substring(payload, parameters->Param2, 8 + paramLen1, paramLen2);
    util_get_substring(payload, parameters->Param3, 12 + paramLen1 + paramLen2, paramLen3);
}

int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer){
    int len1 = strlen(parameters.Param1);
    int len2 = strlen(parameters.Param2);
    int len3 = strlen(parameters.Param3);
    printf("len1: %d\n", len1);
    printf("len2: %d\n", len2);
    printf("len3: %d\n", len3);

    int bufferSize = 20 + len1 + len2 + len3;

    memcpy(buffer, &op, sizeof(op));
    memcpy(buffer+4, &func, sizeof(func));
    memcpy(buffer+8, (uint32_t*)&len1, sizeof(uint32_t));
    memcpy(buffer+12, parameters.Param1, len1*sizeof(uint32_t));
    memcpy(buffer+12+len1, (uint32_t*)&len2, sizeof(uint32_t));
    memcpy(buffer+16+len1, parameters.Param2, len2*sizeof(uint32_t));
    memcpy(buffer+16+len1+len2, (uint32_t*)&len3, sizeof(uint32_t));
    memcpy(buffer+20+len1+len2, parameters.Param3, len3*sizeof(uint32_t));
    buffer[bufferSize] = '\0';

    return bufferSize;
}

