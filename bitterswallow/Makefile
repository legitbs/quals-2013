CC=gcc
CFLAGS=-Wall -lm -fno-stack-protector
SHARED= md2.c md4.c md5.c crc16-ccitt.c crc32.c sum.c fletcher.c adler.c aphash.c rshash.c hashes.c xor.c sha1.c sha2.c sha4.c ed2k.c sub.c murmurhash2.c murmurhash3.c mul.c mod.c div.c cos.c sin.c tan.c ../common/sharedfuncs.c
EXECUTABLE=bs
SOURCES=bitterswallow.c $(SHARED)
INCLUDES=../common/

all:
	$(CC) -I $(INCLUDES) $(SOURCES) $(CFLAGS) -o $(EXECUTABLE)

debug:
	$(CC) $(SOURCES) $(CFLAGS) -DDEBUG -I $(INCLUDES) -o $(EXECUTABLE)
