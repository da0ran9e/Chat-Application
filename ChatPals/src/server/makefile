CC = gcc
CFLAGS = -Wall

all: server

server: server.c
	$(CC) $(CFLAGS) -o server main.c features.c log.c authentication.c connection.c conversations.c relation.c rooms.c ../database/db_connection.c ../database/database.c ../database/sql_queries.c ../utility.c ../protocol/protocol.c -L/usr/include/postgresql/libpq -lpq



