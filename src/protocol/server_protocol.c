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
    // printf("Binary String : \n");
    for (size_t i = 0; i < len; i++) {
        printf("\\x%02X", (unsigned char)string[i]+pos);
    }
    printf("\n");
    char * temp = (char *) malloc((len + 1)*sizeof(char));
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

void getProtocolPayload (const char *message, char * buffer, int size){
    // Extract the payload from the entire of the message
    memcpy(buffer, message + 2*sizeof(uint32_t), size);
}

Parameters getProtocolParameters(const char *payload, Parameters parameters){
    // Extract parameters from the payload 
    uint32_t paramLen1;
    uint32_t paramLen2;
    uint32_t paramLen3;
    memcpy(&paramLen1, payload, sizeof(paramLen1));
    memcpy(&paramLen2, payload + sizeof(uint32_t) + paramLen1, sizeof(paramLen2));
    memcpy(&paramLen3, payload + 2*sizeof(uint32_t) + paramLen1 + paramLen2, sizeof(paramLen3));
    // printf("ParamLen1: %d\n", paramLen1);
    // printf("ParamLen2: %d\n", paramLen2);
    // printf("ParamLen3: %d\n", paramLen3);

    substring(payload, parameters.Param1, 4, paramLen1);
    substring(payload, parameters.Param2, 8+paramLen1, paramLen2);
    substring(payload, parameters.Param3, 12+paramLen1+paramLen2, paramLen3);
    // printf("Param1: %s\n", parameters.Param1);
    // printf("Param2: %s\n", parameters.Param2);
    // printf("Param3: %s\n", parameters.Param3);
    return parameters;
}

void undoTranslation(uint32_t intValue, char charValue, char* undoBuffer, size_t bufferSize) {
    // Copy the integer into the buffer
    memcpy(undoBuffer, &intValue, sizeof(intValue));

    // Copy the character into the buffer after the integer
    memcpy(undoBuffer + sizeof(intValue), &charValue, sizeof(charValue));
}

int generateMessage(uint32_t op, uint32_t func, Parameters parameters, char * buffer){
    int len1 = strlen(parameters.Param1);
    int len2 = strlen(parameters.Param2);
    int len3 = strlen(parameters.Param3);
    printf("ParamLen1: %d\n", len1);
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
    buffer[bufferSize-1] = '\0';

    return bufferSize;
}

int main() {
    // Example binary string
    const char binaryString[] = "\x01\x00\x00\x00\x0f\x00\x00\x00\x0b\x00\x00\x00helloworldA\x06\x00\x00\x00this's\x08\x00\x00\x00protocol";
    const char testBinary[] = {1, 0, 0, 0, 15, 0, 0, 0, 10, 0, 0, 0, 'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', 0, 0, 0, 0, 0, 0, 0, 0, 0 };

    printf("Opcode: %d\n", getProtocolOpcode(binaryString));
    printf("Func: %d\n", getProtocolFunctionCode(binaryString));
    char payload[sizeof(binaryString)];

    getProtocolPayload(binaryString, payload, sizeof(payload));
    Parameters p;
    p = getProtocolParameters(payload, p);
    printf("Param1: %s\n", p.Param1);
    printf("Param2: %s\n", p.Param2);
    printf("Param3: %s\n", p.Param3);

    // // Print the binary string
    // printf("Binary String : \n");
    // for (size_t i = 0; i < sizeof(testBinary); i++) {
    //     printf("\\x%02X", (unsigned char)testBinary[i]);
    // }
    printf("\n");
    printf("Binary String : \n");
    for (size_t i = 0; i < sizeof(binaryString); i++) {
        printf("\\x%02X", (unsigned char)binaryString[i]);
    }
    printf("\n");

    char newBinaryString[45];
    int s = generateMessage(1, 15, p, newBinaryString);
    printf("Binary String %d: \n", s);
    for (size_t i = 0; i < sizeof(newBinaryString); i++) {
        printf("\\x%02X", (unsigned char)newBinaryString[i]);
    }
    printf("\n");


    return 0;
}
