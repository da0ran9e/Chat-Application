#include "../../include/server/feature.h"

int g_clientSockets[MAX_CLIENTS];
char g_clientNames[MAX_CLIENTS][50];

int handle_features(int op, int func, const Parameters params){
    switch (10*func + op)
    {
    case 0: // op 0, func 0
        break;
    case 10: // op 0, func 1
        break;
    case 20: // op 0, func 2
        break;
    case 30: // op 0, func 3
        break;
    case 1:
        break;
    case 11:
        break;
    case 21:
        break;
    case 2:
        break;
    case 12:
        break;
    case 22:
        break;
    case 32:
        break;
    case 3:
        break;
    case 13:
        break;
    default:
        return -1;
        break;
    }
}

int readMessage(const char * binaryString, int size) {
    printf("Opcode: %d\n", getProtocolOpcode(binaryString));
    printf("Func: %d\n", getProtocolFunctionCode(binaryString));
    char payload[size];

    getProtocolPayload(binaryString, payload, sizeof(payload));
    Parameters p;
    p = getProtocolParameters(payload, p);
    
    printf("len1: %d\n", strlen(p.Param1));
    printf("Param1: %s\n", p.Param1);
    printf("len2: %d\n", strlen(p.Param2));
    printf("Param2: %s\n", p.Param2);
    printf("len3: %d\n", strlen(p.Param3));
    printf("Param3: %s\n", p.Param3);

    

    // Print the binary string

    printf("Binary String : \n");
    for (size_t i = 0; i < size; i++) {
        printf("\\x%02X", (unsigned char)binaryString[i]);
    }
    printf("\n");

    return 0;
}

void _init (int port){
    int serverSocket;
    initializeServer(&serverSocket, port);
    runServer(serverSocket);
    close(serverSocket);
}