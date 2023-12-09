CC = gcc
CFLAGS = -c -Wall -g
LDFLAGS = -lpq

all: app

app: app.o
	$(CC) app.o $(LDFLAGS) -o app

app.o: database_connector.c
	$(CC) $(CFLAGS) database_connector.c -o app.o

clean:
	
