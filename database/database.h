#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include "libpq-fe.h"

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