#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Parameters{
    char Param1[1024];
    char Param2[1024];
    char Param3[1024];
} Parameters;

void substring(const char *string, char *buffer, int pos, int len)
{
    char * temp = malloc(len + 1);
    strcpy(temp, string + pos);
    temp[len] = '\0';  // Null-terminate the string
    strcpy(buffer, temp);
}

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

void getProtocolPayload (const char *message, char * buffer){
    // Extract the payload from the entire of the message
    strcpy(buffer, message + 2*sizeof(uint32_t));
}

void getProtocolParameters(const char *payload, Parameters parameters){
    // Extract parameters from the payload
    uint32_t paramLen1;
    uint32_t paramLen2;
    uint32_t paramLen3;
    memcpy(&paramLen1, payload, sizeof(paramLen1));
    memcpy(&paramLen2, payload + sizeof(uint32_t) + paramLen1, sizeof(paramLen2));
    memcpy(&paramLen3, payload + 2*sizeof(uint32_t) + paramLen1 + paramLen2, sizeof(paramLen3));

    substring(payload, parameters.Param1, 4, paramLen1);
    substring(payload, parameters.Param2, 8+paramLen1, paramLen2);
    substring(payload, parameters.Param3, 12+paramLen1+paramLen2, paramLen3);
}

void undoTranslation(uint32_t intValue, char charValue, char* undoBuffer, size_t bufferSize) {
    // Copy the integer into the buffer
    memcpy(undoBuffer, &intValue, sizeof(intValue));

    // Copy the character into the buffer after the integer
    memcpy(undoBuffer + sizeof(intValue), &charValue, sizeof(charValue));
}

int main() {
    // Example binary string
    const char binaryString[] = "\x01\x00\x00\x00\x0f\x00\x00\x00\x05\x00\x00\x00\x68\x65\x6c\x6c\x6f\x00\x00\x00\x00\x00\x00\x00\x00";

    // Print the binary string after undo
    printf("Binary String after Undo: ");
    for (size_t i = 0; i < sizeof(undoBuffer); i++) {
        printf("\\x%02X", (unsigned char)undoBuffer[i]);
    }
    printf("\n");

    return 0;
}
