#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void KeyExpansion(unsigned char *, unsigned int *, int);
void aes_encrypt(unsigned char *, unsigned char *, unsigned int *, int);
void aes_decrypt(unsigned char *, unsigned char *, unsigned int *, int);

char key[] = "thisisnotthekey!";

void encrypt(char *plaintext, char *ciphertext) {
	unsigned int key_schedule[60];

	KeyExpansion(key, key_schedule, 128);
	//aes_encrypt(plaintext, ciphertext, key_schedule, 128);
	aeslite_encrypt(plaintext, ciphertext, key_schedule, 128);

}

void decrypt(char *ciphertext, char *plaintext) {
	unsigned int key_schedule[60];

	KeyExpansion(key, key_schedule, 128);
	//aes_decrypt(ciphertext, plaintext, key_schedule, 128);
	aeslite_decrypt(ciphertext, plaintext, key_schedule, 128);

}

void sendit(int sock, char *buf, unsigned int len) {
	int i = 0;
	int j = 0;
	char plaintext[16];
	char ciphertext[16];
	char output[1600];

	// cowardly refuse to deal with a buffer larger than our output buffer
	if (len > 1600) {
		return;
	}

	while (i < len) {
		bzero(plaintext, 16);
		bzero(ciphertext, 16);
		bcopy(buf+i, plaintext, (len-i < 16) ? (len-i) : 16);

		encrypt(plaintext, ciphertext);

		bcopy(ciphertext, output+j, 16);
		i+=16;
		j+=16;
	}

	// send the number of bytes we're transmitting
	write(sock, &j, 4);

	// send the bytes
	write(sock, output, j);	

}

void sendtext(int sock, char *buf) {
	sendit(sock, buf, strlen(buf));
}

unsigned int receiveit(int sock, char *buf, unsigned int max) {
	char length[8];
	unsigned int recv_len;
	char input[1600];
	char output[1600];
	char ciphertext[16];
	char plaintext[16];
	int i = 0;
	int j = 0;
	int n;
	
	// read in the number of bytes to follow
	read(sock, &recv_len, 4);

	// only receive the max our input buffer can hold
	if (recv_len > 1600) {
		recv_len = 1600;
	}

	// read in the rest of the bytes
	n = read(sock, input, recv_len);
	if (n == 0) {
		// socket is closed
		return(0);
	}

	while (i < n) {
		bzero(plaintext, 16);
		bzero(ciphertext, 16);
		bcopy(input+i, ciphertext, (n-i < 16) ? (n-i) : 16);
		
		decrypt(ciphertext, plaintext);

		bcopy(plaintext, output+j, 16);
		i+=16;
		j+=16;
	}

	bzero(buf, max);
	bcopy(output, buf, (j > max-1) ? max-1 : j);

	return(j);
}
