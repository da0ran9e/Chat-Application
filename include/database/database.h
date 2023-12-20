#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>

#include <postgresql/libpq-fe.h>
#include "../shared/common.h"

enum DBStatus{
    DENY,
    ACCEPT,
    CONFLICT,
    NOTFOUND,
    FULL,
    DONE,
    ERROR
};

PGconn * database_start ();
void database_exit(PGconn *conn);

#endif