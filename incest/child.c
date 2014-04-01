#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <signal.h>
#include <sched.h>

#include <sys/ptrace.h>


void quitter() {
	exit(0);
}

int main(int argc, char ** argv)
{
	int fileno;
	pid_t child;
	char *flagData;
	struct stat statBuf;
	int sockfd;
	char *scbuf;
	char (*func)();

	signal(SIGCHLD, SIG_IGN);
	signal(SIGALRM, quitter);
	alarm(15);

	sockfd = atoi(argv[2]);
	fileno = atoi(argv[1]);
	child = fork();
	if(child == 0) {
		close(fileno);
		scbuf = mmap(0, 0x1000, PROT_READ | PROT_WRITE | PROT_EXEC, 
					 MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
		recv(sockfd, scbuf, 512, 0);
		func = scbuf;
		(*func)();
	}
	close(sockfd);
	fstat(fileno, &statBuf);
	flagData = calloc(1, statBuf.st_size);
	read(fileno, flagData, statBuf.st_size);
	for(;;) {
		sched_yield();
	}
}
