all: common.c aes.c thyself.c client.c
	gcc -m32 -c common.c
	gcc -m32 -c aes.c
	gcc -m32 -c aes_utils.c
	gcc -m32 -c client.c
	gcc -m32 -c thyself.c
	gcc -m32 -o client client.o aes.o common.o
	gcc -m32 -o thyself thyself.o aes.o common.o
	gcc -m32 -o aes_utils aes_utils.o aes.o
