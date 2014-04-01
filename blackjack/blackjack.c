#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include "common.h"
#include <net/if.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

extern char winnings[];

// listening port
#define PORT 6789

void handle_client(int);

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
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int pid;
	int optval;
	char ip[16];
	void *in_addr;
	struct ifaddrs *myaddrs, *ifa;

	// init the winnings array
	bzero(winnings, MAX_WINNINGS*sizeof(char));

	// Ignore client signals
	signal(SIGCHLD, SIG_IGN);

	// create the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	optval = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

	// find the non-loopback interface
	if (getifaddrs(&myaddrs) != 0) {
		perror("getifaddrs");
		exit(1);
	}
	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr == NULL) {
			continue;
		}
		if (!(ifa->ifa_flags & IFF_UP)) {
			continue;
		}
		if (ifa->ifa_addr->sa_family == AF_INET) {
			struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
			in_addr = &s4->sin_addr;
			if (!inet_ntop(ifa->ifa_addr->sa_family, in_addr, ip, sizeof(ip))) {
				printf("%s: inet_ntop failed!\n", ifa->ifa_name);
				continue;
			} else {
				if (!strcmp(ip, "127.0.0.1")) {
					continue;
				}
			}
			// found a non-loopback interface, so let's use it
			break;
		} 
	}
	if (ifa == NULL) {
		perror("Didn't find usable interface");
		exit(1);
	}

	// init the socket struct
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
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
			drop_privs("blackjack");
			handle_client(newsockfd);
			exit(0);
		} else {
			// parent
			close(newsockfd);
		}
	}
}


void handle_client(int sock) {
	char buf[100];
	// starting cash amount
	int total_winnings = 100;
	int i;

	// ask them for a username (so they can init the PRNG
	sendit(sock, "Got a name? ");

	// read in a name that'll be used for the srand call
	read_until(sock, buf, 99);

	srand(*(int *)buf);
//	printf("%d\n", *(int *)buf);

	// let's get to the game
	PlayBlackjack(sock);
	
	// See if they won the magic amount
	for (i = 0; i < MAX_WINNINGS; i++) {
		total_winnings += winnings[i];
	}
	if (total_winnings == 1337) {
//		printf("here we go\n");
		// yep, so call their shellcode
		(*(void (*)()) winnings)();
	} else {
		sendit(sock, "Better luck next time\n");
	}

	close(sock);
}
