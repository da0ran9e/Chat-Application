PROG = Tic-Tac-Toe

CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0`

SRCS = main.c app.c

all: ${PROG}

${PROG}: ${SRCS:.c=.o}
	${CC} -o ${PROG} ${SRCS:.c=.o} ${CFLAGS} ${LIBS}

.c.o:
	${CC} ${CFLAGS} -c $<

clean:
	rm -f ${PROG} ${SRCS:.c=.o}
