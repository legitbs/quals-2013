CC=gcc
CFLAGS=-Wall -z execstack
INCLUDES=../common/
SHARED=../common/sharedfuncs.c
EXECUTABLE=penser
SOURCES=penser.c $(SHARED)

all:
	$(CC) $(SOURCES) $(CFLAGS) -I $(INCLUDES) -o $(EXECUTABLE)

debug:
	$(CC) $(SOURCES) $(CFLAGS) -DDEBUG -I $(INCLUDES) -o $(EXECUTABLE)
