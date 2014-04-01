all: musicman.c common.c 
	gcc -m32 -c musicman.c
	gcc -DVULN -m32 -c common.c
	gcc -m32 -o musicman musicman.o common.o -lm
