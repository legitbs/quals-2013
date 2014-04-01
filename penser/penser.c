#include "sharedfuncs.h"

#include <sys/mman.h>
#include <wchar.h>
#include <iconv.h>

//int parcheck( char *, int );
int ff( int connfd );
int ascii_to_unicode( char * in, int in_sz, char *out, int out_sz);

int port = 8273;

int main( int argc, char **argv )
{
	int sockfd = SetupSock( port, AF_INET, "eth0" );

	accept_loop( sockfd, ff, "penser" );

	return 0;
}

int ff( int connfd )
{
		  size_t size = 0;
		  int retval = 0;
		  char *buffer = NULL;
		  size_t wide_len = 0;
		  char *wide_buff = NULL;
		  void (*func)();
          alarm(0xf);
		  retval = recv_stream( connfd, (char *)&size, sizeof(int));

		  if ( retval == -1 )
		  {
					 close( connfd );
					 return -1;
		  }

		  // ensure that size is valid
		  if ( size > 0x1000 )
		  {
					 send_data( connfd, "Invalid length.\n", 16 );
					 close( connfd );
					 return -1;
		  }

		  buffer = (char*)malloc(size);

		  if ( buffer == NULL )
		  {
					 close( connfd );
					 return -1;
		  }

		  retval = recv_stream( connfd, (char *)buffer, size );

		  wide_len = size * 2;

		  wide_buff = mmap( NULL, wide_len, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

		  memcpy(wide_buff, buffer, size);

		  retval = ascii_to_unicode( buffer, size, wide_buff, wide_len );

		  if ( retval == -1 )
		  {
					 return -1;
		  }

		  free( buffer );

		  func = (void (*)())wide_buff;
		  (void)(*func)();

		  munmap( wide_buff,  wide_len);
		  close(connfd);

		  return 0;
}

int ascii_to_unicode( char * in, int in_sz, char *out, int out_sz)
{
		  int out_count = 0;
		  int in_count = 0;

		  if ( in == NULL || in_sz == 0 || out == NULL || out_sz == 0 )
		  {
					 return -1;
		  }

		  if ( out_sz >> 1 < in_sz )
		  {
					 return -1;
		  }

		  memset( out, 0x00, out_sz );

		  while ( in_count < in_sz )
		  {
					 if ( in[in_count] == 0x00 )
					 {
								return 0;
					 }

					 if ( (in[in_count] <0x20 || in[in_count] > 0x7f) && in[in_count] !=0x0a )
					 {
								return -1;
					 }

					 out[out_count] = in[in_count];
					 out_count += 2;
					 in_count++;					 
		  }

		  return in_count;
}

// Accepts a buffer the high bit of each byte is a parity bit.
// Expects odd parity. Fixes the buffer in place
// Returns 0 on success, -1 otherwise
/*int parcheck( char * data, int len )
{
		  char byte = 0;
		  int databit = 0;
		  int databyte = 0;
		  int counter = 0;
		  int bit = 0;
		  int i = 0;
		  int parity = 0;
		  int shift = 0;
		  char *new_data = NULL;

		  if ( len == 0 )
					 return -1;

		  if (data == NULL )
					 return -1;

		  new_data = (char *)malloc(len);

		  if (new_data==NULL)
					 return -1;

		  memset(new_data, 0x00, len);

		  while( counter < len )
		  {
					 byte = data[counter];
					
					 parity = 0;
 
					 for( i = 0; i < 8; i++ )
					 {
								parity += (byte & ( 1 << i )) >> i;
					 }
					 
					 if ( parity & 1 )
					 {
								for ( i = 6; i >= 0; i--)
								{
										  databyte = databit >> 3;
										  bit = databit % 8;	  
										  shift = bit - ( 7 - i );

										  if ( shift < 0 )
										  {
													 new_data[databyte] |= ( (byte & (1 << i) ) << abs(shift) );
										  } else
										  {
													 new_data[databyte] |= ( (byte & (1 << i)) >> shift );
										  }

										  databit++;
								}
					 }
					 else
					 {
								DEBUG_PRINT("Failed parity\n");
								return -1;
					 }

					 counter++;
		  }

		  memcpy(data, new_data, len);

		  free(new_data);

		  return 0;
}*/
