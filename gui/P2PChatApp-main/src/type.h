#ifndef TYPE_H
#define TYPE_H

#include <netinet/in.h>
#include <time.h>
#include "config.h"
#include <gtk/gtk.h>

// Buffer size
#define USRLEN 50
#define PWDLEN 50
#define MSGLEN 500          // MSGLEN + 2 x USRLEN < PAYLOADSIZE
#define PAYLOADSIZE 1000

// Network message type
// Type of data in payload
#define ACCOUNT 0
#define CHATMSG 1
#define DISCONNECT -11
#define ONLINELIST 4
// Client request
#define LOGIN 2
#define LOGOUT 3
#define REGISTER 7
// Register state
#define REGSUCCESS 77
#define REGDUP -77
// HI message
#define HI 5
// Client request to connect
#define CONNECT 6
// Connect response
#define ACCEPT 66
#define REJECT -66
#define CANCEL -67
// Verify user
#define VERIFY 8
#define VALID 88
#define INVALID -88
// Ending message
#define ACK 9 
#define END -1
// Bad request
#define BADREQ -126
// Internal error
#define ERROR -127

// Login state
#define LOGINSUCCESS 22
#define LOGINALREADY -22
#define UNREGISTERED -2
#define WRONGIDENT -3
#define LOCKEDACC -4
#define DISABLED -5
#define NONE -6

// Account status
#define INACTIVE -1
#define LOCKED 0
#define ACTIVE 1

// Chat communication state
#define READY 0
#define NA -1

// Client state
#define CHATTING 0
#define CONNECTING 1
#define EXIT -2

// Message direction
#define SEND 0
#define RECV 1

typedef struct {
    char usr[USRLEN];
    char pwd[PWDLEN];
    char token[TOKENLEN+1];
    struct sockaddr_in usrAddr;
    char status;
    char logCnt;
    char offCnt;
    GtkVBox* chatVBox;
    GtkWindow* chatWnd;
} account_t;

typedef struct {
    char from[USRLEN];
    char token[TOKENLEN+1];
    char to[USRLEN];
    char txt[MSGLEN];
    time_t at;
} chat_msg_t;

typedef struct {
    char type;
    unsigned short len;
    char payload[PAYLOADSIZE];
} net_msg_t;

#endif // !TYPE_H
