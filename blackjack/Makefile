all: blackjack.c common.c
	gcc -m32 -z execstack -c blackjack.c
	gcc -m32 -z execstack -c common.c
	gcc -m32 -z execstack -c exploit.c
	gcc -m32 -z execstack -o blackjack blackjack.o common.o
	gcc -m32 -z execstack -o exploit exploit.o common.o
