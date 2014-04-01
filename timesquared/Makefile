all: timescubed.c ../common/sharedfuncs.c ../common/sharedfuncs.h
	gcc -m32 -I ../common -c ../common/sharedfuncs.c
	gcc -m32 -c timescubed.c
	gcc -m32 -o timescubed timescubed.o sharedfuncs.o
