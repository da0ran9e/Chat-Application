CC = gcc
CFLAGS = -Wall

all: server log

server: server.c
	$(CC) $(CFLAGS) -o server server.c

log: log.c
	$(CC) $(CFLAGS) -o log log.c

clean:
	rm -f server log
