#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"

enum LoginStatus{
    DENY,
    ACCEPT
};

enum RegisterStatus{
    USERNAME_CONFLICT,
    SERVER_FULL,
    DONE
};

enum PasswordInChangeStatus{
    DONE,
    DENY
};

enum AddFriendStatus{
    DONE,
    ERROR
};

enum DeleteFiendStatus{
    DONE,
    ERROR
};

enum CreateRoomStatus{
    DONE,
    ERROR
};

enum AddMemberStatus{
    DONE,
    MEMBER_CONFLICT,
    ROOM_FULL
};

enum RemoveMemberStatus{
    DONE,
    MEMBER_NOTFOUND,
};

PGconn * database_start ();
void database_exit(PGconn *conn);

#endif