#include "sharedfuncs.h"

int SetupSock( int port, int type, const char *iface)
{
	int sockfd;
	int optval = 1;

	DEBUG_PRINT("In SetupSock\n");

	// Seed rand
	srand( time(NULL) );

    // Ignore client signals
    signal(SIGCHLD, SIG_IGN);

	sockfd = socket( type, SOCK_STREAM, 0 );

	if ( sockfd == -1 )
	{
        printf("[ERROR] socket() failed %x\n", errno);
		exit( -1 );
	}		

	if ( setsockopt( sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1 )
	{
        printf("[ERROR] setsockopt() failed %x\n", errno);
		exit( -1 );
	}

	if ( bindtoif( iface, sockfd, type, port ) != 0 )
	{
		exit( -1 );
	}

	if ( listen( sockfd, 0x14 ) != 0 )
	{
		exit( -1 );
	}

	return sockfd;
}

void proc_handler( int signum )
{
	int status;

	while (wait4( -1, &status, 1, NULL ) > 0 ){} 

	return;
}

int bindtoif( const char *ifname, int sockfd, int type, int port )
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
        printf("[ERROR] getifaddrs() failed %x\n", errno);
		exit( -1 );
   }

	walker = ifs;

	while ( walker )
	{
		if ( walker->ifa_addr->sa_family == type )
		{
			if ( strcmp( walker->ifa_name, ifname ) == 0)
			{
				if ( type == AF_INET )
				{
					((struct sockaddr_in *)(walker->ifa_addr))->sin_port = htons( port);

					retval = bind( sockfd, walker->ifa_addr, sizeof(struct sockaddr_in) );

					if ( retval != 0 )
					{
                        printf("[ERROR] ipv4 bind() failed %x\n", errno);
						exit(-1);
					}
				} else if (type == AF_INET6 )
				{
					((struct sockaddr_in6 *)(walker->ifa_addr))->sin6_port = htons(port);

					retval = bind( sockfd, walker->ifa_addr, sizeof( struct sockaddr_in6) );

					if ( retval != 0 )
					{
                        printf("[ERROR] ipv6 bind() failed %x\n", errno);
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
        printf("[ERROR] Failed to find interface: %s\n", ifname);
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

int drop_privs( const char * name )
{
	struct passwd * pw = NULL;

	if ( name == NULL )
	{
		exit(-1);
	}

	pw = getpwnam( name );

	if ( pw == NULL )
	{
	    printf("[ERROR] getpwnam() failed %x\n", errno);	
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
        printf("[ERROR] initgroups() failed %x\n", errno);
		return -1;
	}

	if ( setresgid( pw->pw_gid, pw->pw_gid, pw->pw_gid ) != 0 )
	{
		printf("[ERROR] setresgid() failed %x\n", errno);
		return -1;
	}

	if ( setresuid( pw->pw_uid, pw->pw_uid, pw->pw_uid ) != 0 )
	{
		printf("[ERROR] setresuid() failed %x\n", errno);
		return -1;
	}

	if ( procgid != pw->pw_gid )
	{
		if ( setgid( procgid ) != -1 )
		{
			printf("[ERROR] setgid() failed %x\n", errno);
			return -1;
		}

		if ( setegid( procgid ) != -1 )
		{
			printf("[ERROR] setegid() failed %x\n", errno);
			return -1;
		}
	}

	if ( procuid != pw->pw_uid )
	{
		if ( setuid( procuid ) != -1 )
		{
			printf("[ERROR] setuid() failed %x\n", errno);
			return -1;
		}

		if ( seteuid( procuid ) != -1 )
		{
			printf("[ERROR] seteuid() failed %x\n", errno);
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
		printf("[ERROR] chdir() failed %x\n", errno);
		return -1;
	}

	return 0;
}

int accept_loop( int sockfd, fork_function ff, const char *name )
{
	int connsock = 0;
	struct sockaddr_in6 sin6;
	socklen_t sinlen = sizeof(sin6);
	pid_t child = 0;
	int retval = 0;

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
                drop_privs(name);
				retval = ff( connsock );
				close( connsock );
				exit(retval);
			}
		}

	}

	return 0;
}
