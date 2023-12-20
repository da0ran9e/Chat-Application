#ifndef DATABASE_H
#define DATABASE_H

#include "queries.h"

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