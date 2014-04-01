#include "sharedfuncs.h"

int SetupSock( int port, int type )
{
	int sockfd;
	int optval = 1;

	DEBUG_PRINT("In SetupSock\n");

	// Seed rand
	srand( time(NULL) );

	if ( signal( 0x14, proc_handler ) == SIG_ERR )
	{
		exit(-1);
	} 

	sockfd = socket( type, SOCK_STREAM, 0 );

	if ( sockfd == -1 )
	{
		exit( -1 );
	}		

	if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1 )
	{
		exit( -1 );
	}

	if ( bindtoif( "eth0", sockfd, type, port ) != 0 )
	{
		printf("failed to bind\n");
		exit( -1 );
	}

	if ( listen( sockfd, 0x14 ) != 0 )
	{
		printf("Failed to listen\n");
		exit( -1 );
	}

	return sockfd;
}

void proc_handler( int signum )
{
	int status;

	DEBUG_PRINT("In proc_handler\n");

	while (wait4( -1, &status, 1, NULL ) > 0 ){} 

	return;
}

int bindtoif( char *ifname, int sockfd, int type, int port )
{
	int retval = 0;
   struct ifaddrs *ifs;
	struct ifaddrs *walker = NULL;

	if ( ifname == NULL )
	{
		return -1;
	}

   retval = getifaddrs( &ifs );

   if ( retval != 0 )
	{
		exit( -1 );
	}

	walker = ifs;

	while ( walker )
	{
		if ( walker->ifa_addr->sa_family == type )
		{
			DEBUG_PRINT("Found correct family\n");
			if ( strcmp( walker->ifa_name, ifname ) == 0)
			{
				DEBUG_PRINT("found if %s\n", ifname);

				if ( type == AF_INET )
				{
					((struct sockaddr_in *)(walker->ifa_addr))->sin_port = htons( port);

					retval = bind( sockfd, walker->ifa_addr, sizeof(struct sockaddr_in) );

					if ( retval != 0 )
					{
						DEBUG_PRINT("failed to bind ipv4: %x\n", errno);
						exit(-1);
					}
				} else if (type == AF_INET6 )
				{
					((struct sockaddr_in6 *)(walker->ifa_addr))->sin6_port = htons(port);

					retval = bind( sockfd, walker->ifa_addr, sizeof( struct sockaddr_in6) );

					if ( retval != 0 )
					{
						DEBUG_PRINT("failed to bind ipv6\n");
						exit(-1);
					}
				}
				break;
			}
		}
		walker = walker->ifa_next;
	}
			
	freeifaddrs( ifs );

	if ( walker )
	{
		return 0;
	} else
	{
		return -1;
	}
}

int send_string( int sockfd, char *buffer)
{
	int size = 0;

	if ( buffer == NULL )
	{
		return -1;
	}

	size = strlen( buffer );

	return send_data( sockfd, buffer, size );
}

int send_data( int sockfd, char * buffer, int size )
{
	int sentbytes = 0;

	if ( buffer == NULL )
	{
		return -1;
	}

	if ( size == 0 )
	{
		return 0;
	}

	while ( sentbytes < size )
	{
		sentbytes += send( sockfd, buffer + sentbytes, size - sentbytes, 0 );

		if ( sentbytes < 0 )
		{
			return -1;
		}
	}

	return sentbytes;
}

int recv_until( int sockfd, char * buffer, int size, char c )
{
	int readbytes = 0;
	int counter = 0;
	char recvchar = 0;

	if (buffer == NULL)
	{
		return -1;
	}

	if ( size == 0 )
	{
		return 0;
	}

 	do
	{
		readbytes = recv( sockfd, &recvchar, 1, 0 );

		if ( readbytes < 0 )
		{
			DEBUG_PRINT("recv_until(): readbytes: %x\n", readbytes);
			return -1;
		}

		buffer[counter] = recvchar;

		counter++;

	} while ( (counter < size) && (recvchar != c) );

	return counter;
}

int recv_stream( int sockfd, char * buffer, int size )
{
		  int readbytes = -1;

		  if ( buffer == NULL )
		  {
					 return -1;
		  }

		  if ( size == 0 )
		  {
					 return 0;
		  }

		  readbytes = recv( sockfd, buffer, size, 0 );

		  return readbytes;
}

int recvdata( int sockfd, char * buffer, int size)
{
	int readbytes = -1;
	int counter = 0;

	if (buffer == NULL)
	{
		return -1;
	}

	if ( size == 0 )
	{
		return 0;
	}

	while ( (counter < size) && (readbytes != 0) )
	{
		readbytes = recv( sockfd, buffer + counter, 1, 0 );

		if ( readbytes < 0 )
		{
			return -1;
		}

		counter++;
	}

	return counter;
}

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

int accept_loop( int sockfd, fork_function ff )
{
	int connsock = 0;
	struct sockaddr_in6 sin6;
	socklen_t sinlen = sizeof(sin6);
	pid_t child = 0;
	int retval = 0;

	DEBUG_PRINT("In accept_loop()\n");

	memset( &sin6, 0x00, sinlen);

	while( 1 )
	{
		connsock = accept( sockfd, (struct sockaddr *)&sin6, &sinlen );

		if ( connsock != -1 )
		{
			child = fork( );

			if ( child != 0 )
			{
				close(connsock);
			} else if ( child == 0 )
			{
				close( sockfd );
        #ifndef DEBUG
				alarm(0x0f);
        #endif
				retval = ff( connsock );
				printf("finished ff\n");
				close( connsock );
				printf("closed connsock\n");
				exit(retval);
			}
		}

	}

	return 0;
}
