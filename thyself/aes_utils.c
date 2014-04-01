#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void aeslite_break(unsigned char plaintext[], unsigned char ciphertext[], unsigned char out[])
{
   unsigned char plain[4][4];
   unsigned char cipher[4][4];

   // Copy the input to the plain.
   build_state_array_from_string(plaintext, plain);
   build_state_array_from_string(ciphertext, cipher);
   
   InvMixColumns(cipher);
   InvShiftRows(cipher);
   InvSubBytes(cipher);

   // Copy the state to the output array
   out[0] = plain[0][0] ^ cipher[0][0];
   out[1] = plain[1][0] ^ cipher[1][0];
   out[2] = plain[2][0] ^ cipher[2][0];
   out[3] = plain[3][0] ^ cipher[3][0];
   out[4] = plain[0][1] ^ cipher[0][1];
   out[5] = plain[1][1] ^ cipher[1][1];
   out[6] = plain[2][1] ^ cipher[2][1];
   out[7] = plain[3][1] ^ cipher[3][1];
   out[8] = plain[0][2] ^ cipher[0][2];
   out[9] = plain[1][2] ^ cipher[1][2];
   out[10] = plain[2][2] ^ cipher[2][2];
   out[11] = plain[3][2] ^ cipher[3][2];
   out[12] = plain[0][3] ^ cipher[0][3];
   out[13] = plain[1][3] ^ cipher[1][3];
   out[14] = plain[2][3] ^ cipher[2][3];
   out[15] = plain[3][3] ^ cipher[3][3];
}

void usage(void) {
	printf("aes_utils usage\n");
	printf("break cipher and find the encryption key\n");
	printf("aes_utils -b <16 bytes of plaintext> <16 bytes matching ciphertext in hex (so 32 characters)>\n\n");
	printf("encrypt (-e) or decrypt(-d) the provided text\n");
	printf("aes_utils -e <plaintext> <ascii key (16 bytes)>\n");
	printf("aes_utils -d <ciphertext in hex> <ascii key 16 bytes)>\n");
	exit(0);
}
int main(int argc, char **argv) {
	unsigned char plaintext[16];
	unsigned char ciphertext[16];
	unsigned char key[16];
	int i, j, k;
	unsigned char byte[3];
	int len;
	unsigned int key_schedule[60];

	if (argc != 4) {
		usage();
	}

	if (!strcmp(argv[1],"-b")) {
		// being asked to find the encryption key for a single-rouned AES encryption algo
		if (strlen(argv[2]) != 16) {
			printf("plaintext must be 16 bytes long\n");
			exit(0);
		}
		if (strlen(argv[3]) != 32) {
			printf("ciphertext must be 32 characters long\n");
			exit(0);
		}

//		bzero(plaintext, 16);
//		bcopy(argv[2], plaintext, 16);

		// convert ciphertext input string to hex values
		j = 0;
		for (i = 0; i < 32; i+=2) {
			byte[0] = argv[3][i];
			byte[1] = argv[3][i+1];
			byte[2] = '\0';
			ciphertext[j++] = (unsigned char)strtoul(byte, NULL, 16);
		}

		aeslite_break(argv[2], ciphertext, key);

		printf("encryption key is: ");
		for (i = 0; i < 16; i++) {
			printf("%02x", key[i] & 0xff);
		}
		printf("\n");
	} else if (!strcmp(argv[1],"-e")) {
		len = strlen(argv[2]);

		KeyExpansion(argv[3], key_schedule, 128);
		printf("ciphertext: ");
		i = 0;
		while (i < len) {
			bzero(plaintext, 16);
			bzero(ciphertext, 16);
			bcopy(argv[2]+i, plaintext, ((len-i < 16) ? (len-i) : 16));
			aeslite_encrypt(plaintext, ciphertext, key_schedule, 128);
			for (k = 0; k < 16; k++) {
				printf("%02x", ciphertext[k] & 0xff);
			}
			i+=16;
			
		}
		printf("\n");
	} else if (!strcmp(argv[1], "-d")) {
		len = strlen(argv[2]);
		KeyExpansion(argv[3], key_schedule, 128);
		printf("plaintext: ");
		i = 0;
		while (i < len) {
			bzero(plaintext, 16);
			bzero(ciphertext, 16);
			j = 0;
			for (k = i; k < i+32; k+=2) {
				byte[0] = argv[2][k];
				byte[1] = argv[2][k+1];
				byte[2] = '\0';
				ciphertext[j++] = (unsigned char)strtoul(byte, NULL, 16);
			}
			aeslite_decrypt(ciphertext, plaintext, key_schedule, 128);
			for (k = 0; k < 16; k++) {
				printf("%c", plaintext[k]);
			}
			i+=32;
			
		}
		printf("\n");

	} else {
		usage();
	}
}
