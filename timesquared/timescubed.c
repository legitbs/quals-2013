#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>


typedef int bool;
#define True 1
#define False 0
#define T 10
#define W 10
#define H 10
#define D 10
#define before 0
#define after 1
#define inFrontOf 2
#define behindOf 3
#define aboveOf 4
#define belowOf 5
#define leftOf 6
#define rightOf 7
#define maxGuess 13
#define numDim 4
#define PORT 1338
#define HAZ_LOW 65
#define HAZ_RANGE 10
//char b[T*W*H*D+1];
//char g[maxGuess+1];
//int nb[numDim*2][2];

void get_neighbor(int p,char b[T*W*H*D+1], int nb[numDim*2][2]){
	//[T-1][W][H][D] is before
	if (p/(W*H*D)>0){
		nb[before][1] = b[p-(W*H*D)];
		nb[before][0] = p-(W*H*D);
	}
	else {
		nb[before][1] = 0;
		nb[before][0] = 0;
	} 
	//[T+1][W][H][D] is after
	if (p+(W*H*D)<(T*W*H*D)){
		nb[after][1] = b[p+(W*H*D)];
		nb[after][0] = p+(W*H*D);
	}
	else {
		nb[after][1] = 0;
		nb[after][0] = 0;
	}
	//[T][W-1][H][D] is inFrontOf
	if ((p%(W*H*D))/(H*D)>0){
		nb[inFrontOf][1] = b[p-(H*D)];
		nb[inFrontOf][0] = p-(H*D);
	}
	else {
		nb[inFrontOf][1] = 0;
		nb[inFrontOf][0] = 0;
	}
	//[T][W+1][H][D] is behindOf
	if ((p%(W*H*D))+(H*D)<(W*H*D)){
		nb[behindOf][1] = b[p+(H*D)];
		nb[behindOf][0] = p+(H*D);
	}
	else{
		nb[behindOf][1] = 0;
		nb[behindOf][0] = 0;
	}
	//[T][W][H-1][D] is aboveOf
	if (((p%(H*D))/D)>0){
		nb[aboveOf][1] = b[p-D];
		nb[aboveOf][0] = p-D; 
	}
	else{
		nb[aboveOf][1]=0;
		nb[aboveOf][0]=0;
	}
	//[T][W][H+1][D] is belowOf
	if (((p%(H*D))+D)<(H*D)){
		nb[belowOf][1] = b[p+D];
		nb[belowOf][0] = p+D;
	}
	else{
		nb[belowOf][1] =0;
		nb[belowOf][0] =0;
	}
	//[T][W][H][D-1] is leftOf
	if (((p%D)-1)>=0){
		nb[leftOf][1] = b[p-1];
		nb[leftOf][0] = p-1;
	}
	else{
		nb[leftOf][1]=0;
		nb[leftOf][0]=0;
	}		
	//[T][W][H][D+1] is rightOf
	if (((p%D)+1)<D){
		nb[rightOf][1] = b[p+1];
		nb[rightOf][0] = p+1;
	}
	else{
		nb[rightOf][1]=0;
		nb[rightOf][0]=0;
	}

	return;
	
}

void generate_array ( char b[T*W*H*D] ) {
	unsigned int randSeed;
	int i;
	int n = T*W*H*D;
	int randData = open ("/dev/urandom", O_RDONLY);
	read (randData, &randSeed, sizeof randSeed);
	srand (randSeed);
	b[n] = 0;
	for (i = 0; i < (n); i++){
		if (i==0){
			b[0] = (char)((rand()%HAZ_RANGE) +HAZ_LOW);
		}
		else{
			b[i] = (char)(( rand()%94 ) + 32);
		}
	}
	return;

}
int validate_guess ( char b[T*W*H*D] ,char g[maxGuess] ) {
	int i, j, k;
	int bPos = ((W*H*D)+(H*D)+(D)+1);
	int nb[numDim*2][2];
	for (i=0; i<maxGuess; i+=1) {
		get_neighbor ( bPos,b, nb );
		//printf("\ng[%i] %c\n",i,g[i]);
		//printf("nb[0][0] %i [0][1] %i, nb[1][0] %i [1][1] %i,nb[2][0] %i [2][1] %i,nb[3][0] %i [3][1] %i,nb[4][0] %i [4][1] %i,nb[5][0] %i [5][1] %i,nb[6][0] %i [6][1] %i,nb[7][0] %i [7][1] %i\n",nb[0][0],nb[0][1],nb[1][0],nb[1][1],nb[2][0],nb[2][1],nb[3][0],nb[3][1],nb[4][0],nb[4][1],nb[5][0],nb[5][1],nb[6][0],nb[6][1],nb[7][0],nb[7][1]);
		if ( g[i] == 0 ) {
 			i = maxGuess;
			for ( j=0; j<8; j+=1){
				//printf(" j is %i",j);
				if ((nb[j][1] > b[0] ) && (nb[j][1] != 0)) {return (False);}
			}
			return (True);
		}
		
		k = ( (b[bPos] ^ g[i]) %8);
		//printf ("our move is %i",k);
		if (( nb[k][1] >= b[0] ) && ( nb[k][1] >0 )){
			b[bPos] = 0;
			bPos = nb[k][0];
		}
		else{
			//printf("invalid move");
			return (False);
		}
	}
	return (False);
}

void dostuff(int sock){
	//todo:make TWHD appear dynamic by taking def'd const x numargs? maybe 
	int i,strSize,n;
	char g[maxGuess];
	char b[T*W*H*D];
	FILE *fp;
	generate_array(b);
	char *buf = NULL;
	char *buf2 = NULL; 
	char *buf3 = NULL;
	buf3 = (char*) malloc (sizeof(char)*(100));
	sprintf(buf3, "\nGene Ray is thinking of a number..\nIt is between 0 and \n");
	send (sock, buf3, strlen(buf3), 0);
	send (sock, b, strlen(b), 0);
	sprintf(buf3, "\nCan you guess it?\n");
	send (sock, buf3, strlen(buf3), 0);
	memset(buf3,0,100);
	alarm(10);
	n = recv(sock,g,maxGuess,0);
//	printf("%i",n);
	g[n] == 0;
	
	if ( validate_guess(b,g) == (True)) {
//		printf("g: %s\n", g);
		fp=fopen("/home/timescubed/key", "r");
		if (fp){
			fseek(fp,0,SEEK_END);
			strSize = ftell(fp);
			rewind(fp);
			buf = (char*) malloc (sizeof(char) * (strSize+1) );
			buf2 =(char*) malloc (sizeof(char) * (strSize+20) ); 
			fread(buf,sizeof(char),strSize,fp);
			buf[strSize+1] = '\0';
			sprintf(buf2,"\nThe key is: %s\n",buf);
			int r;
			//r = write (sock, buf2, strlen(buf2));	
			r = send (sock, buf2, strlen(buf2), 0);	
			syncfs(sock);
//			sleep(2);
//			printf("solution(%d): %s\n", r, buf2);
//       		fflush(stdout);
			free(buf);
			free(buf2);	
		}
		else{
//			printf("unable to open file\n");
 //       		fflush(stdout);
			free(buf);
			sprintf(buf3,"Oh Shit!\nkeyfiles are missing, locate nearest adult for assistance!\n*This is not a drill!\n");
			send (sock, buf3, strlen(buf3), 0);
			free (buf3);
  		}
		return;
	}
//	printf("wrong\n");
//        fflush(stdout);
	sprintf (buf3,"Wrong\n");
	send (sock, buf3, strlen(buf3), 0);
	free(buf3);
	return;
}

int main ( int argc, char **argv ) {
	//copy pasta socket code from interwebs**
    int sockfd, newsockfd, portno, clilen,pid;
    struct sockaddr_in serv_addr, cli_addr;
    int optval = 1;

    signal(SIGCHLD, SIG_IGN);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
    {
        perror("ERROR opening socket");
        exit(1);
    }
    setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
 
    if (bind(sockfd, (struct sockaddr *) &serv_addr,
                          sizeof(serv_addr)) < 0)
    {
         perror("ERROR on binding");
         exit(1);
    }

    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    while (1) 
    {
        newsockfd = accept(sockfd, 
                (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
        {
            perror("ERROR on accept");
            exit(1);
        }
        /* Create child process */
        pid = fork();
        if (pid < 0)
        {
            perror("ERROR on fork");
	    exit(1);
        }
        if (pid == 0)  
        {
            /* This is the client process */
            close(sockfd);
            dostuff(newsockfd);
//            fprintf(stderr, "exiting\n");
//            fflush(stderr);
            exit(0);
        }
        else
        {
            close(newsockfd);
        }
    } /* end of while */
}
