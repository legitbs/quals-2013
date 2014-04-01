CC=gcc
CFLAGS=-Wall -z execstack
INC1=../common/
INC2=../hjalloc/
INC=$(INC1) $(INC2)
INCLUDES=-I $(INC1) -I $(INC2)
ALLOC_SHARED=../hjalloc/alloc.c ../hjalloc/alloc_fl_block.c ../hjalloc/alloc_lal_block.c ../hjalloc/create_heap.c ../hjalloc/free_fl_block.c ../hjalloc/free_lal_block.c ../hjalloc/hjfree.c
SHARED=../common/sharedfuncs.c $(ALLOC_SHARED)
EXECUTABLE=yolo
SOURCES=yolo.c $(SHARED)

all:
	$(CC) $(INCLUDES) $(SOURCES) $(CFLAGS) -o $(EXECUTABLE)

debug:
	$(CC) $(SOURCES) $(DEBUG) $(CFLAGS) -DDEBUG $(INCLUDES) -o $(EXECUTABLE)
