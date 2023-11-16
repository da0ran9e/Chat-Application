# change application name here (executable output name)
TARGET=GUI

# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall

PTHREAD=-pthread -lX11

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) -export-dynamic

OBJS=	GUI.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)

GUI.o: src/GUI.c
	$(CC) -c $(CCFLAGS) src/GUI.c $(GTKLIB) -o GUI.o

clean:
	rm -f *.o $(TARGET)