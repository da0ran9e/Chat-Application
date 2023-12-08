#include<stdio.h>
#include<stdlib.h>
#include<postgresql/libpq-fe.h>

int main(int argc, char const *argv[])
{
	const char *conninfo;
	PGconn *conn;

	//Step1: setting connection string, default: bdname=postgres
	conninfo = "dbname = postgres";

	//Step2: make a connection to database
	conn = PQconnectdb(conninfo);
	if (PQstatus(conn) != CONNECTION_OK){
		printf("Connection of database failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}

	printf("Host: %s\n", PQport(conn));
	printf("ServerPath: %s\n", PQhost(conn));
	printf("Database: %s\n", PQdb(conn));
	printf("UID: %s\n", PQuser(conn));
	printf("PID: %d\n", PQbackendPID(conn));

	int ver = PQserverVersion((const PGconn *) conn);
	int verMajor = ver/10000;
	int verMinor = ver%10000;
	printf("Version: %d.%d\n", verMajor, verMinor);

	PQfinish(conn);

	return 0;
}