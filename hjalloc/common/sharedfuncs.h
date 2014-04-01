#ifndef __SHARED_FUNCS__
#define __SHARED_FUNCS__

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <grp.h>
#include <ctype.h>
#include <stdint.h>
#include <fcntl.h>

#ifdef DEBUG
#define DEBUG_PRINT(...) fprintf( stderr, __VA_ARGS__);
#else
#define DEBUG_PRINT(...) 
#endif

typedef int (*fork_function)(int connfd);

// Use this to create and return a socket
int SetupSock( int port, int type );

// Signal handler
void proc_handler( int signum );

// Allows the binding to a specific interface
int bindtoif(char * ifname, int sockfd, int type, int port );

// Receive the amount data specified
int recvdata( int sockfd, char *buffer, int size );

// Receive data until the specified size or specified character
int recv_until( int sockfd, char * buffer, int size, char c );

// Receive a single buffer of data of a max size
int recv_stream( int sockfd, char * buffer, int size );

// Send a buffer with length specified by size
int send_data( int sockfd, char *buffer, int size );

// Sends a string. Calculates the length based upon strlen
int send_string( int sockfd, char *string );

// Drop privileges to the specified user
int drop_privs( char * user );

// Sets the necessary user information for drop_privs
int init_user( struct passwd * pw );

// Loop for accepting connections and forking execution
int accept_loop( int sockfd, fork_function ff);

#endif
