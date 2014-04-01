all: linked.c ../common/
	gcc -I ../common -m32 -z execstack -c ../common/sharedfuncs.c
	gcc -m32 -z execstack -c linked.c
	gcc -m32 -z execstack -o linked linked.o sharedfuncs.o
