#ifndef __COMMON__
#define __COMMON__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

void md2( const unsigned char *input, size_t ilen, unsigned char output[16] );
void md4( const unsigned char *input, size_t ilen, unsigned char output[16] );
void md5( const unsigned char *input, size_t ilen, unsigned char output[16] );
void crc16_ccitt( const unsigned char *input, size_t ilen, unsigned char output[2] );
void crc32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void sum8( const unsigned char *input, size_t ilen, unsigned char output[1] );
void mul8( const unsigned char *input, size_t ilen, unsigned char output[1] );
void mul16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void mul32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void sum16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void sum32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void sub8( const unsigned char *input, size_t ilen, unsigned char output[1] );
void sub16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void sub32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void mod8( const unsigned char *input, size_t ilen, unsigned char output[1] );
void mod16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void mod32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void div8( const unsigned char *input, size_t ilen, unsigned char output[1] );
void div16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void div32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void cos128( const unsigned char *input, size_t ilen, unsigned char output[16] );
void acos128( const unsigned char *input, size_t ilen, unsigned char output[16] );
void sin128( const unsigned char *input, size_t ilen, unsigned char output[16] );
void asin128( const unsigned char *input, size_t ilen, unsigned char output[16] );
void tan128( const unsigned char *input, size_t ilen, unsigned char output[16] );
void atan128( const unsigned char *input, size_t ilen, unsigned char output[16] );
void fletcher16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void fletcher32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void adler8( const unsigned char *input, size_t ilen, unsigned char output[1] );
void adler16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void adler32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void aphash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void rshash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void jshash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void pjwhash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void elfhash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void bkdrhash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void sdbmhash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void djbhash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void dekhash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void bphash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void fnvhash( const unsigned char *input, size_t ilen, unsigned char output[4] );
void xor8( const unsigned char *input, size_t ilen, unsigned char output[1] );
void xor16( const unsigned char *input, size_t ilen, unsigned char output[2] );
void xor32( const unsigned char *input, size_t ilen, unsigned char output[4] );
void sha1( const unsigned char *input, size_t ilen, unsigned char output[20] );
void sha224( const unsigned char *input, size_t ilen, unsigned char output[32] );
void sha256( const unsigned char *input, size_t ilen, unsigned char output[32] );
void sha384( const unsigned char *input, size_t ilen, unsigned char output[64] );
void sha512( const unsigned char *input, size_t ilen, unsigned char output[64] );
void murmurhash2( const unsigned char *input, size_t ilen, unsigned char output[4]);
void murmurhash3_x86_32( const unsigned char *input, size_t ilen, unsigned char output[4]);
void murmurhash3_x86_128( const unsigned char *input, size_t ilen, unsigned char output[4]);
void murmurhash3_x64_128( const unsigned char *input, size_t ilen, unsigned char output[16]);
// edonkey
void ed2k( const unsigned char *input, size_t ilen, unsigned char output[16] );
#endif
