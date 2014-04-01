#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>

#define IP "127.0.0.1"
#define PORT 2345

#define LINE_LEN 1000

// buffer to hold data from server
#define BUF_LEN 1030
char BUF[BUF_LEN];

int main(int argc, char **argv) {
	int sockfd, newsockfd, portno, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	char line[LINE_LEN];
	unsigned int n;
	int pid;
	fd_set readfd;
	int maxfd;
	struct timeval timeout;

	// get the IP from the user (in case it changes during the game for some reason)
	if (argc != 2) {
		printf("client <IP>\n");
		exit(-1);
	}

	// create the socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("ERROR opening socket");
		exit(1);
	}

	// init the socket struct
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(PORT);
 
	// bind to that port
	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("ERROR on connect");
		exit(1);
	}

	while (1) {
		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
		FD_ZERO(&readfd);
		FD_SET(sockfd, &readfd);
		FD_SET(STDIN_FILENO, &readfd);
		if (select(sockfd+1, &readfd, NULL, NULL, &timeout) < 0) {
			close(sockfd);
			exit(1);
		}
		if (FD_ISSET(sockfd, &readfd)) {
			// process input from server
			if (!receiveit(sockfd, line, LINE_LEN)) {
				close(sockfd);
				exit(0);
			}

			// deal with any secret text we might have been sent
			if (!strncmp(line, "SECRET", 6)) {
				bzero(BUF, BUF_LEN);
				//strncpy(BUF, line+6, BUF_LEN-1);
				bcopy(line, BUF, BUF_LEN-1);
			} else {
				printf("%s", line);
				fflush(stdout);
			}
		} else if (FD_ISSET(STDIN_FILENO, &readfd)) {
			fgets(line, LINE_LEN-1, stdin);
			sendtext(sockfd, line);
		}
	}
}
