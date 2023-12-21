#include "../../include/database/database.h"

PGconn * database_start (){
    PGconn *conn;

    // Establish a connection to the database
    conn = PQconnectdb("dbname=chat user=postgres password=anquynh123 host=localhost");

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(1);
    }
    return conn;
}

void database_exit(PGconn *conn){
    PQfinish(conn);
}