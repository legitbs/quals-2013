#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

// buffer to hold the shellcode
#define BUF_LEN 1030
char BUF[BUF_LEN];

// listening port
#define PORT 2345

// Flag file (the answer to the challenge)
#define FLAG "/home/services/thyself_key"

// Encryption Key file
#define KEY "/home/services/thyself_encryption_key"

// Key for secret encryption
//#define SECRET_KEY "DamnIhatecrypto!"
char SECRET_KEY[17];

void handle_client(int);
void read_flag(void);
void read_key(void);

int drop_privs( char * name )
{
	struct passwd * pw = NULL;

	if ( name == NULL )
	{
		exit(-1);
	}

	pw = getpwnam( name );

	if ( pw == NULL )
	{
		exit(-1);
	}

	if ( init_user( pw ) != 0 )
	{
		exit(-1);
	}

	return 0;
}

int init_user( struct passwd * pw )
{
	uid_t procuid = 0;
	gid_t procgid = 0;

	if (pw == NULL )
	{
		return -1;
	}

	procuid = getuid( );
	procgid = getgid( );

	if ( initgroups( pw->pw_name, pw->pw_gid ) != 0 )
	{
		return -1;
	}

	if ( setresgid( pw->pw_gid, pw->pw_gid, pw->pw_gid ) != 0 )
	{
		printf("setresgid failed\n");
		return -1;
	}

	if ( setresuid( pw->pw_uid, pw->pw_uid, pw->pw_uid ) != 0 )
	{
		printf("setresuid failed\n");
		return -1;
	}

	if ( procgid != pw->pw_gid )
	{
		if ( setgid( procgid ) != -1 )
		{
			printf("setgid failed\n");
			return -1;
		}

		if ( setegid( procgid ) != -1 )
		{
			printf("setegid failed\n");
			return -1;
		}
	}

	if ( procuid != pw->pw_uid )
	{
		if ( setuid( procuid ) != -1 )
		{
			printf("setuid failed\n");
			return -1;
		}

		if ( seteuid( procuid ) != -1 )
		{
			printf("seteuid failed\n");
			return -1;
		}
	}

	if ( getgid( ) != pw->pw_gid )
	{
		return -1;
	}

	if ( getegid( ) != pw->pw_gid )
	{
		return -1;
	}

	if ( getuid( ) != pw->pw_uid )
	{
		return -1;
	}

	if ( geteuid( ) != pw->pw_uid )
	{
		return -1;
	}

	if ( chdir( pw->pw_dir ) != 0 )
	{
		printf("chdir failed\n");
		return -1;
	}

	return 0;
}

int main(void) {
	int sockfd, newsockfd, clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int pid;
	int optval;

        // Ignore client signals
        signal(SIGCHLD, SIG_IGN);

	// read in the key and the flag
	read_key();
	read_flag();

	// create the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

        optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	// init the socket struct
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(PORT);
 
	// bind to that port
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on binding");
		exit(1);
	}

	// listen for new connections
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0) {
			perror("ERROR on accept");
			exit(1);
		}

		// fork to handle the new connection
		pid = fork();
		if (pid < 0) {
			perror("ERROR on fork");
			exit(1);
		}
		if (pid == 0) {
			// handle the new client
			close(sockfd);
			drop_privs("thyself");
			handle_client(newsockfd);
			exit(0);
		} else {
			// parent
			close(newsockfd);
		}
	}
}

void read_key(void) {
	FILE *in;

	bzero(SECRET_KEY, 17);

	if ((in = fopen(KEY, "r")) == NULL) {
		printf("Failed to open key file: %s\n", KEY);
		exit(-1);
	}

	fread(SECRET_KEY, 1, 16, in);

	fclose(in);

}

void read_flag(void) {
	FILE *in;
	char keytext[1024];
	int keylen;
	unsigned int key_schedule[60];
	char plaintext[16];
	char ciphertext[16];
	int i = 0;
	int j = 0;

	bzero(keytext, 1024);

	// open the key file
	if ((in = fopen(FLAG, "r")) == NULL) {
		printf("Failed to open flag file: %s\n", FLAG);
		exit(-1);
	}

	// read in the key
	keylen = fread(keytext, 1, 1024, in);

	// store the prefix into the key buffer
	bcopy("SECRET", BUF, 6);
	j+=6;

	// encrypt the key
	KeyExpansion(SECRET_KEY, key_schedule, 128);
	//KeyExpansion("1234567890123456", key_schedule, 128);
	while (i < keylen) {
		bzero(plaintext, 16);
		bzero(ciphertext, 16);
		bcopy(keytext+i, plaintext, (keylen-i < 16) ? (keylen-i) : 16);

		aeslite_encrypt(plaintext, ciphertext, key_schedule, 128);

		bcopy(ciphertext, BUF+j, 16);
		i+=16;
		j+=16;
	}

	fclose(in);
}

void handle_client(int sock) {
	int n;
	char line[1024];
	int command_count = 10000;
	char plaintext[17];
	char ciphertext[17];
	char hex[40];
	int i;
	unsigned int key_schedule[60];

	bzero(hex, 40);

	printf("got a client\n");

	sendtext(sock, "Welcome to the key server.\n");
	sendtext(sock, "? for help.\n");
	sendtext(sock, "key-server% ");
	// secretly send the actual key back to the client
	sendit(sock, BUF, BUF_LEN);

	// interact with the client
        while (n = receiveit(sock, line, 1023)) {
		command_count++;

		if (line[strlen(line)-1] == '\n') {
			line[strlen(line)-1] = '\0';
		}

		if (!strncasecmp(line, "ls ", 3)) {
			sendtext(sock, "key-server\n");
			sendtext(sock, "key\n");
		} else if (!strcasecmp(line, "ls")) {
			sendtext(sock, "key-server\n");
			sendtext(sock, "key\n");
		} else if (!strcasecmp(line, "id")) {
			sendtext(sock, "uid=1000(key-server) gid=1000(key-server) groups=1000(key-server)\n");
		} else if (!strcasecmp(line, "whoami")) {
			sendtext(sock, "key-server\n");
		} else if (!strncasecmp(line, "cat key", 7)) {
			sendtext(sock, "Oh, ok...here you go.  Just kidding...if only it were that easy\n");
		} else if (!strcasecmp(line, "cat")) {
			sendtext(sock, "need an argument\n");
		} else if (!strncasecmp(line, "cd", 2)) {
			sendtext(sock, "permission denied\n");
		} else if (!strncasecmp(line, "su ", 3)) {
			sendtext(sock, "haha...yeah right\n");
		} else if (!strncasecmp(line, "sudo", 4)) {
			sendtext(sock, "nope...keep looking\n");
		} else if (!strncasecmp(line, "fuck", 4)) {
			sendtext(sock, "right back at ya!\n");
		} else if (!strncasecmp(line, "vi ", 3)) {
			sendtext(sock, "nice idea...but no\n");
		} else if (!strncasecmp(line, "pwd", 3)) {
			sendtext(sock, "/home/key-server\n");
		} else if (!strncasecmp(line, "AAAAAAAAAAA", 11)) {
			sendtext(sock, "You're barking up the wrong tree\n");
		} else if (!strcasecmp(line, ";")) {
			sendtext(sock, "Nope...\n");
		} else if (!strncasecmp(line, "scp ", 4)) {
			sendtext(sock, "You can't have the server binary...why can't you be happy with just the client?\n");
		} else if (!strcasecmp(line, ">")) {
			sendtext(sock, "denied\n");
		} else if (!strncasecmp(line, "help", 4)) {
			sendtext(sock, "ls, cat, id, whoami, pwd, aeslite_encrypt\n");
		} else if (!strncasecmp(line, "?", 1)) {
			sendtext(sock, "ls, cat, id, whoami, pwd, aeslite_encrypt\n");
		} else if (!strncasecmp(line, "./key-server", 12)) {
			sendtext(sock, "already running...but thanks for checking\n");
		} else if (!strncasecmp(line, "key-server", 10)) {
			sendtext(sock, "already running...but thanks for checking\n");
		} else if (!strncasecmp(line, "kill", 4)) {
			sendtext(sock, "It wouldn't be any fun if the server wasn't running\n");
		} else if (!strncasecmp(line, "pkill", 5)) {
			sendtext(sock, "It wouldn't be any fun if the server wasn't running\n");
		} else if (!strncasecmp(line, "aeslite_encrypt ", 16)) {
			if (strlen(line) == 32) {
				KeyExpansion(SECRET_KEY, key_schedule, 128);
				strncpy(plaintext, line+16, 16);
				aeslite_encrypt(plaintext, ciphertext, key_schedule, 128);
				sprintf(hex, "Here you go: SECRET");
				for (i = 0; i < 16; i++) {
					sprintf(hex, "%s%02x", hex, ciphertext[i] & 0xff);
				}
				sprintf(hex, "%s\n", hex);
				sendtext(sock, hex);
			} else {
				sendtext(sock, "aeslite_encrypt <16 characters>\n");
			}
		} else if (!strcasecmp(line, "aeslite_encrypt")) {
			sendtext(sock, "aeslite_encrypt <16 characters>\n");
			sendtext(sock, "uses default key for encryption.  Returns printable representation of encryption for your convenience.\n");
		} else if (!strcasecmp(line, "logout")) {
			sendtext(sock, "Ok...it's been real. Bye.\n");
			close(sock);
			return;
		} else if (!strcasecmp(line, "quit")) {
			sendtext(sock, "Ok...it's been real. Bye.\n");
			close(sock);
			return;
		} else if (!strcasecmp(line, "exit")) {
			sendtext(sock, "Ok...it's been real. Bye.\n");
			close(sock);
			return;
		} else if (!strncasecmp(line, "ps -", 4)) {
			sendtext(sock, "0 S key-server      14818 14815  0  80   0 - 14614 poll_s 21:27 pts/0    00:00:00 key-server\n");
			sendtext(sock, "0 R key-server      24828 14818  0  80   0 -  5590 -      21:27 pts/3    00:00:00 jsh\n");
			snprintf(line, 1023, "0 R key-server      %.05d 24828  0  80   0 -  3397 -      21:27 pts/3    00:00:00 ps\n", command_count);
			sendtext(sock, line);
		} else if (!strncasecmp(line, "ps", 2)) {
			sendtext(sock,"  PID TTY          TIME CMD\n");
			sendtext(sock,"14188 pts/0    00:00:00 key-server\n");
			sendtext(sock,"24828 pts/3    00:00:00 jsh\n");
			snprintf(line, 1023, "%.05d pts/3    00:00:00 ps\n", command_count);
			sendtext(sock, line);	
		} else {
			sendtext(sock, "unknown command\n");
		}

		if ((command_count % 10) == 0) {
			sendtext(sock, "\nTired of looking at the server side yet?\n");
		}
		sendtext(sock, "key-server% ");
        }

	close(sock);
	exit(0);
}
