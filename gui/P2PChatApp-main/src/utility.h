#ifndef UTILITY_H
#define UTILITY_H

#include "type.h"

char* upperize (char* dest, char* str);

char* lowerize (char* dest, char* str);

char* genToken (char* tokenBuff);

int rangeRand (int a, int b);

char* removeChar (char* str, char c);

char* trimLeft (char* s);

char* trimRight (char* s);

char* trim (char* s);

void clearCL ();

void clearBuffer ();

int serializeNetMsg (char* msgBuff, const net_msg_t* msg);

net_msg_t* deserializeNetMsg (net_msg_t* msg, char* msgBuff);

int serializeOnlineAccount (char* accBuff, const account_t* acc);

account_t* deserializeOnlineAccount (account_t* acc, char* accBuff);

int serializeChatAccount (char* accBuff, const account_t* acc);

account_t* deserializeChatAccount (account_t* acc, char* accBuff);

int serializeAuthAccount (char* accBuff, const account_t* acc);

account_t* deserializeAuthAccount (account_t* acc, char* accBuff);

int serializeSignupAccount (char* accBuff, const account_t* acc);

account_t* deserializeSignupAccount (account_t* acc, char* accBuff);

int serializeAccessAccount (char* accBuff, const account_t* acc);

account_t* deserializeAccessAccount (account_t* acc, char* accBuff);

int serializeChatMsg (char* msgBuff, const chat_msg_t* msg);

chat_msg_t* deserializeChatMsg (chat_msg_t* msg, char* msgBuff, int msgLen);

int serializeChatMsgStorage (char* msgBuff, const chat_msg_t* msg);

chat_msg_t* deserializeChatMsgStorage (chat_msg_t* msg, char* msgBuff, int msgLen);

int makeHIMsg (char *msgBuff, char *usrBuff, char *tokenBuff);

char* extractHIMsg (char *usrBuff, char *tokenBuff, char *msgBuff);

char* getMsgFilePath (char* buf, char* usr);

#endif // !UTILITY_H
