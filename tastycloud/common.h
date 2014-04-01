#ifndef __COMMON__
#define __COMMON__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

void crc32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void md5( const unsigned char *input, size_t ilen, unsigned char output[4] );
#endif
