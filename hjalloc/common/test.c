#include "sharedfuncs.h"

int testfunc(int connfd);

int main(int argc, char **argv)
{
	int sockfd = SetupSock( 12345 );

	drop_privs( "testuser" );

	accept_loop( sockfd, testfunc );

	return 0;
}

int testfunc(int connfd)
{
	char buff[1024];
	memset(buff, 0x00,1024);
	int size = 1;

	size = recv_until( connfd, buff, 1024, '\xa');

	printf("sending data\n");
	send_data( connfd, buff, size );
	printf("data send\n");
	return 0;
}
