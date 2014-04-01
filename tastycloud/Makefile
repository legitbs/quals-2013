CC=gcc
CFLAGS=-Wall -lm -s
INCLUDES=../common/
SHARED=../common/sharedfuncs.c
EXECUTABLE=tastycloud
SOURCES=tastycloud.c md5.c $(SHARED)

all:
	$(CC) $(SOURCES) $(CFLAGS) -I $(INCLUDES) -o $(EXECUTABLE)

debug:
	$(CC) $(SOURCES) $(CFLAGS) -DDEBUG -I $(INCLUDES) -o $(EXECUTABLE)
