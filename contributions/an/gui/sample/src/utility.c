#include "utility.h"
#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include "type.h"

#define TOKENLETTERLEN 52

const char* tokenLetter = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
const char* online_account_t_format_str = "%s %hu %u %hu";
const char* chat_account_t_format_str = "%s %s %hu %u %hu";
const char* auth_account_t_format_str = "%s %s %hu %u %hu";
const char* signup_account_t_format_str = "%s %s";
const char* access_account_t_format_str = "%s %s";
const char* hi_msg_format_str = "%s %s";

// [a, b)
int rangeRand (int a, int b) {
    return random() % (b-a) + a;
}

char* upperize (char* dest, char* str) {
    char* p = str;
    char* it = dest;
    
    for (; *p != 0; ++p, ++it) *it = toupper(*p);
    *it = 0;

    return dest;
}

char* lowerize (char* dest, char* str) {
    char* p = str;
    char* it = dest;
    
    for (; *p != 0; ++p, ++it) *it = tolower(*p);
    *it = 0;

    return dest;
}

char* genToken (char tokenBuff[TOKENLEN+1]) {
    char sec[20];
    sprintf(sec, "%ld", time(NULL));
    int seclen = strlen(sec);
    srand(clock());
    for (int i = 0; i < TOKENLEN - seclen; ++i) {
        tokenBuff[i] = tokenLetter[rangeRand(0, TOKENLETTERLEN)];
    }
    tokenBuff[TOKENLEN-seclen] = 0;
    memcpy(tokenBuff + TOKENLEN - seclen, sec, seclen);
    tokenBuff[TOKENLEN] = 0;

    return tokenBuff;
}

char* removeChar(char* str, char c) {
    int j, n = strlen(str);
    for (int i = j = 0; i < n; i++)
        if (str[i] != c)
            str[j++] = str[i];
 
    str[j] = '\0';

    return str;
}

char* trimLeft (char* s) {
    int i = 0;
    int j = 0;
    while (s[i] != 0 && isspace(s[i])) ++i;
    while (s[i] !=0 ) {
        s[j++] = s[i++];
    }
    s[j] = 0;

    return s;
}

char* trimRight (char* s) {
    int i = strlen(s);

    if (i == 0)
        return s;
    --i;
    while (i >= 0) {
        if (isspace(s[i])) {
            s[i] = 0;
        } else {
            break;
        }
        --i;
    }
    return s;
}

char* trim (char* s) {
    trimLeft(s);
    trimRight(s);

    return s;
}

void clearCL () {
    #ifdef WINDOWS
        system("cls");
    #else
        system ("clear");
    #endif
}

void clearBuffer () {
    char c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int serializeNetMsg (char* msgBuff, const net_msg_t* msg) {
    memset(msgBuff, 0, sizeof(net_msg_t));
    msgBuff[0] = msg->type;
    msgBuff[1] |= (msg->len >> 8);
    msgBuff[2] |= msg->len;
    memcpy(msgBuff+3, msg->payload, msg->len);

    return msg->len + 3;
}

net_msg_t* deserializeNetMsg (net_msg_t* msg, char* msgBuff) {
    memset(msg, 0, sizeof(*msg));
    msg->type = msgBuff[0];
    msg->len = ((unsigned short) msgBuff[1] << 8) | (unsigned char) msgBuff[2];
    memcpy(msg->payload, msgBuff+3, msg->len);

    return msg;
}

int serializeOnlineAccount (char* accBuff, const account_t* acc) {
    sprintf(accBuff, online_account_t_format_str, acc->usr,
                                                acc->usrAddr.sin_family, acc->usrAddr.sin_addr.s_addr, acc->usrAddr.sin_port);

    return strlen(accBuff)+1;       // include NULL terminate char
}

account_t* deserializeOnlineAccount (account_t* acc, char* accBuff) {
    memset(acc, 0, sizeof(*acc));
    sscanf(accBuff, online_account_t_format_str, acc->usr,
                                                &acc->usrAddr.sin_family, &acc->usrAddr.sin_addr.s_addr, &acc->usrAddr.sin_port);
    
    return acc;
}

int serializeChatAccount (char* accBuff, const account_t* acc) {
    sprintf(accBuff, chat_account_t_format_str, acc->usr, acc->token,
                                                acc->usrAddr.sin_family, acc->usrAddr.sin_addr.s_addr, acc->usrAddr.sin_port);

    return strlen(accBuff)+1;       // include NULL terminate char
}

account_t* deserializeChatAccount (account_t* acc, char* accBuff) {
    memset(acc, 0, sizeof(*acc));
    sscanf(accBuff, chat_account_t_format_str, acc->usr, acc->token,
                                                &acc->usrAddr.sin_family, &acc->usrAddr.sin_addr.s_addr, &acc->usrAddr.sin_port);
    
    return acc;
}

int serializeAuthAccount (char* accBuff, const account_t* acc) {
    sprintf(accBuff, auth_account_t_format_str, acc->usr, acc->pwd,
                                                acc->usrAddr.sin_family, acc->usrAddr.sin_addr, acc->usrAddr.sin_port);

    return strlen(accBuff)+1;       // include NULL terminate char
}

account_t* deserializeAuthAccount (account_t* acc, char* accBuff) {
    memset(acc, 0, sizeof(*acc));
    sscanf(accBuff, auth_account_t_format_str, acc->usr, acc->pwd,
                                            &acc->usrAddr.sin_family, &acc->usrAddr.sin_addr, &acc->usrAddr.sin_port);
    
    return acc;
}

int serializeSignupAccount (char* accBuff, const account_t* acc) {
    sprintf(accBuff, signup_account_t_format_str, acc->usr, acc->pwd);

    return strlen(accBuff)+1;       // include NULL terminate char
}

account_t* deserializeSignupAccount (account_t* acc, char* accBuff) {
    memset(acc, 0, sizeof(*acc));
    sscanf(accBuff, signup_account_t_format_str, acc->usr, acc->pwd);
    
    return acc;
}

int serializeAccessAccount (char* accBuff, const account_t* acc) {
    sprintf(accBuff, access_account_t_format_str, acc->usr, acc->token);

    return strlen(accBuff)+1;       // include NULL terminate char
}

account_t* deserializeAccessAccount (account_t* acc, char* accBuff) {
    memset(acc, 0, sizeof(*acc));
    sscanf(accBuff, access_account_t_format_str, acc->usr, acc->token);

    return acc;
}

int serializeChatMsg (char* msgBuff, const chat_msg_t* msg) {
    int fromLen = strlen(msg->from);
    int tokenLen = TOKENLEN;
    int toLen = strlen(msg->to);
    int txtLen = strlen(msg->txt);
    int timeSize = sizeof(msg->at);
    
    int totalSize = 0;
    memcpy(msgBuff + totalSize, msg->from, fromLen);
    totalSize += fromLen;
    msgBuff[totalSize++] = 0;

    memcpy(msgBuff + totalSize, msg->token, tokenLen);
    totalSize += tokenLen;
    msgBuff[totalSize++] = 0;

    memcpy(msgBuff + totalSize, msg->to, toLen);
    totalSize += toLen;
    msgBuff[totalSize++] = 0;
    
    memcpy(msgBuff + totalSize, msg->txt, txtLen);
    totalSize += txtLen;
    msgBuff[totalSize++] = 0;

    memcpy(msgBuff + totalSize, &msg->at, timeSize);
    totalSize += timeSize;
    
    return totalSize;
}

chat_msg_t* deserializeChatMsg (chat_msg_t* msg, char* msgBuff, int msgLen) {
    memset(msg, 0, sizeof(*msg));
    int i = 0;
    int j = 0;

    while (i < msgLen && msgBuff[i] != 0) {
        msg->from[j++] = msgBuff[i];
        ++i;
    }
    msg->from[j] = 0;

    ++i;
    j = 0;
    while (i < msgLen && msgBuff[i] != 0) {
        msg->token[j++] = msgBuff[i];
        ++i;
    }
    msg->token[j] = 0;

    ++i;
    j = 0;
    while (i < msgLen && msgBuff[i] != 0) {
        msg->to[j++] = msgBuff[i];
        ++i;
    }
    msg->to[j] = 0;

    ++i;
    j = 0;
    while (i < msgLen && msgBuff[i] != 0) {
        msg->txt[j++] = msgBuff[i];
        ++i;
    }
    msg->txt[j] = 0;

    ++i;
    memcpy(&msg->at, msgBuff + i, sizeof(msg->at));
    i += sizeof(msg->at);

    if (i != msgLen) {
        printf("Error: Invalid chat message string format\n");
    }
    
    return msg;
}

int serializeChatMsgStorage (char* msgBuff, const chat_msg_t* msg) {
    int fromLen = strlen(msg->from);
    int toLen = strlen(msg->to);
    int txtLen = strlen(msg->txt);
    int timeSize;
    char atStr[20];
    sprintf(atStr, "%ld", msg->at);
    timeSize = strlen(atStr);
    
    int totalSize = 0;
    memcpy(msgBuff + totalSize, msg->from, fromLen);
    totalSize += fromLen;
    msgBuff[totalSize++] = 0;

    memcpy(msgBuff + totalSize, msg->to, toLen);
    totalSize += toLen;
    msgBuff[totalSize++] = 0;
    
    memcpy(msgBuff + totalSize, msg->txt, txtLen);
    totalSize += txtLen;
    msgBuff[totalSize++] = 0;

    memcpy(msgBuff + totalSize, atStr, timeSize);
    totalSize += timeSize;
    msgBuff[totalSize] = 0;

    return totalSize;
}

chat_msg_t* deserializeChatMsgStorage (chat_msg_t* msg, char* msgBuff, int msgLen) {
    memset(msg, 0, sizeof(*msg));
    int i = 0;
    int j = 0;
    char buf[20];

    while (i < msgLen && msgBuff[i] != 0) {
        msg->from[j++] = msgBuff[i];
        ++i;
    }
    msg->from[j] = 0;

    ++i;
    j = 0;
    while (i < msgLen && msgBuff[i] != 0) {
        msg->to[j++] = msgBuff[i];
        ++i;
    }
    msg->to[j] = 0;

    ++i;
    j = 0;
    while (i < msgLen && msgBuff[i] != 0) {
        msg->txt[j++] = msgBuff[i];
        ++i;
    }
    msg->txt[j] = 0;

    ++i;
    j = 0;
    while (i < msgLen && msgBuff[i] != 0) {
        buf[j++] = msgBuff[i];
        ++i;
    }
    buf[j] = 0;
    msg->at = atoi(buf);

    if (i != msgLen) {
        printf("Error: Invalid chat message storage string format %d %d\n", i, msgLen);
    }
    
    return msg;
}

int makeHIMsg (char *msgBuff, char *usrBuff, char *tokenBuff) {
    net_msg_t msg;
    
    msg.type = HI;
    snprintf(msg.payload, PAYLOADSIZE, hi_msg_format_str, usrBuff, tokenBuff);
    msg.len = strlen(msg.payload);

    return serializeNetMsg(msgBuff, &msg);
}

char* extractHIMsg (char *usrBuff, char *tokenBuff, char *msgBuff) {
    net_msg_t msg;

    deserializeNetMsg(&msg, msgBuff);
    msg.payload[msg.len+1] = 0;
    sscanf(msg.payload, hi_msg_format_str, usrBuff, tokenBuff);

    return usrBuff;
}

char* getMsgFilePath (char* buf, char* usr) {
    strcpy(buf, msgFilePath);
    strcat(buf, "_");
    strcat(buf, usr);
    strcat(buf, ".txt");

    return buf;
}