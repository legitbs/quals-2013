all: incest.c child.c sharedfuncs.c
	gcc -o child child.c
	gcc -c sharedfuncs.c
	gcc -c incest.c
	gcc -o incest incest.o sharedfuncs.o
