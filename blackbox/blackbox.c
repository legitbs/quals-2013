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
#include <sys/mman.h>

// buffer pointer to hold the shellcode (mmap'd later)
#define BUF_LEN 2048
char *BUF;

// listening port
#define PORT 1234

void handle_client(int);

char LastMove = ' ';

#define WIDTH 100
#define HEIGHT 100
int dungeon[WIDTH][HEIGHT];

int pos_x = WIDTH/2;
int pos_y = HEIGHT/2;

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

int main(int argc, char **argv) {
	int sockfd, newsockfd, clilen;
	struct sockaddr_in serv_addr, cli_addr;
	int n;
	int pid;
	int optval;

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
			if (argc == 1) {
				drop_privs("blackbox");
			}
			handle_client(newsockfd);
			exit(0);
		} else {
			// parent
			close(newsockfd);
		}
	}
}

void sendit(int sock, char *line) {
	write(sock, line, strlen(line));
}

void read_until(int sock, char *buffer, int max) {
	int i = 0;
	char local_char[2];
	int n;

	alarm(5);
	while ((n = read(sock,local_char,1)) == 1) {
		if (local_char[0] == '\n' || i == max) {
			break;
		}
		buffer[i++] = local_char[0];
	}
	buffer[i] = '\0';
}

char DescribeRoom2(int sock) {
	char local_buf[256];
	int A = 0;
	int N = 0;
	int S = 0;
	int E = 0;
	int W = 0;
	int invalid = -1;

	// keep looping if they send us invalid commands
	while (invalid) {
		sendit(sock, "-------------------------------------------------------------------\n");

		// stick to valid options
		if (invalid == 1) {
			sendit(sock, "Stick with valid options...\n");
		} 
		invalid = 1;

		sendit(sock, "You're standing in a room with exits in these directions: \n");
	
		if (pos_y != 0) {
			if (dungeon[pos_x][pos_y-1]) {
				sendit(sock, "North\n");
				N = 1;
			}
		}
		if (pos_y != HEIGHT-1) {
			if (dungeon[pos_x][pos_y+1]) {
				sendit(sock, "South\n");
				S = 1;
			}
		}
		if (pos_x != 0) {
			if (dungeon[pos_x-1][pos_y]) {
				sendit(sock, "West\n");
				W = 1;
			}
		}
		if (pos_x != WIDTH-1) {
			if (dungeon[pos_x+1][pos_y]) {
				sendit(sock, "East\n");
				E = 1;
			}
		}

		// if this position has a '2', it contains a troll...and their chance for glory
		if (dungeon[pos_x][pos_y] == 2) {
			A = 1;
			sendit(sock, "There is a troll in this room!\n");
			sendit(sock, "You can run away like a scared little baby through one of the exits above, or you can ATTACK.\n");
			sendit(sock, "What would you like to do (");
			if (N) {
				sendit(sock, "N");
			}
			if (S) {
				if (N) {
					sendit(sock, ",");
				}
				sendit(sock,"S");
			}
			if (E) {
				if (N || S) {
					sendit(sock, ",");
				}
				sendit(sock,"E");
			}
			if (W) {
				if (N || S || E) {
					sendit(sock, ",");
				}
				sendit(sock,"W");
			}
			sendit(sock, ",A");
			sendit(sock, ")? ");

			// read their decision...with an overflow
			read_until(sock,local_buf,296);

			// see what they decided to do
			if (local_buf[0] == 'N' && N) {
				invalid = 0;
				sendit(sock, "Coward...\n");
				pos_y--;
			} else if (local_buf[0] == 'S' && S) {
				invalid = 0;
				sendit(sock, "Coward...\n");
				pos_y++;
			} else if (local_buf[0] == 'E' && E) {
				invalid = 0;
				sendit(sock, "Coward...\n");
				pos_x++;
			} else if (local_buf[0] == 'W' && W) {
				invalid = 0;
				sendit(sock, "Coward...\n");
				pos_x--;
			} else if (local_buf[0] == 'A') {
				// this will let them out of the DescribeRoom2 function and allow them 
				// to use that nicely overflowed saved EIP they just hopefully set
				sendit(sock, "Hell yeah, let's go!\n");
				invalid = 0;
			}
		} else {
			sendit(sock, "Which direction would you like to go (");
			if (N) {
				sendit(sock, "N");
			}
			if (S) {
				if (N) {
					sendit(sock, ",");
				}
				sendit(sock,"S");
			}
			if (E) {
				if (N || S) {
					sendit(sock, ",");
				}
				sendit(sock,"E");
			}
			if (W) {
				if (N || S || E) {
					sendit(sock, ",");
				}
				sendit(sock,"W");
			}
			sendit(sock, ")? ");

			// see what they decided to do...not overflowable
			read_until(sock,local_buf,256);
			if (local_buf[0] == 'N' && N) {
				invalid = 0;
				pos_y--;
			} else if (local_buf[0] == 'S' && S) {
				invalid = 0;
				pos_y++;
			} else if (local_buf[0] == 'E' && E) {
				invalid = 0;
				pos_x++;
			} else if (local_buf[0] == 'W' && W) {
				invalid = 0;
				pos_x--;
			}
		}	
	}

	return(local_buf[0]);
}

void init_dungeon() {
	int i,j;

	for (i = 0; i < WIDTH; i++) {
		for (j = 0; j < HEIGHT; j++) {
			// see if this is a valid room
			if (rand()/(double)RAND_MAX > 0.3) {
				// see if this room has a troll
				if (rand()/(double)RAND_MAX > 0.9) {
					// yep
					dungeon[i][j] = 2;
				} else {
					// nope
					dungeon[i][j] = 1;
				}
			}
		}
	}

	// set the intial square as a valid room
	dungeon[WIDTH/2][HEIGHT/2] = 1;
}

void handle_client(int sock) {

	srand(time(0));

	BUF = mmap(0, BUF_LEN, PROT_EXEC | PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE , -1, 0);

	bzero(BUF,BUF_LEN);

	// initialize the dungeon array
	init_dungeon();

	// ask them for a username (a good place for them to store their shellcode)
	sendit(sock, "What shall I call you?\n");

	// safely read in a username or something to BUF so they can load their shellcode
	read_until(sock, BUF, BUF_LEN);

	// let them roam around until they find a troll and decide to attack
	while (DescribeRoom2(sock) != 'A');

	// if they got here, they didn't manage to overwrite EIP...so kick them out
	sendit(sock, "\nSorry, ");
	sendit(sock, BUF);
	sendit(sock, "\nYou died...better luck next time.\n");

	close(sock);

}
