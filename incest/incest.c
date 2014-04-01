#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "sharedfuncs.h"

int ff(int);

int main (int argc, char ** argv)
{
	int sockFd = SetupSock(65535, AF_INET, "eth0");
	signal(SIGCHLD, SIG_IGN);
	accept_loop(sockFd, ff);
}

int ff(int sockFd) {
	struct stat statbuf;
	FILE *keyFile;
	char * keybuf;
	char fileStr[4];
	char sockStr[4];

	keyFile = fopen("key", "r");
	
	if(keyFile == NULL) {
		send_string(sockFd, "Could not open key.");
		return -1;
	}
	drop_privs("maw");
	pid_t child;
	child = fork();
	if(child == 0) {
		sprintf(fileStr, "%d", fileno(keyFile));
		sprintf(sockStr, "%d", sockFd);
		execl("/home/services/sis", "sis", fileStr, sockStr, NULL);
	}
	fclose(keyFile);
}
