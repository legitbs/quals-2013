/*
 * Simple MD5 implementation
 *
 * Compile with: gcc -o md5 md5.c
 */
#include "sharedfuncs.h"
#include <math.h>
#include "common.h"
#include <sys/stat.h>
#include <sys/types.h>

// r specifies the per-round shift amounts

uint32_t r[48];
 
// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

int ff(int connfd);
int wrap( uint8_t *data, uint32_t len, uint8_t *out );
inline void to_bytes(uint32_t val, uint8_t *bytes) __attribute__((always_inline));
inline uint32_t to_int32(uint8_t *bytes) __attribute__((always_inline));
inline void md6(uint8_t *initial_msg, size_t initial_len, uint8_t *digest) __attribute__((always_inline));
inline void make_r( ) __attribute__((always_inline));
 
inline void to_bytes(uint32_t val, uint8_t *bytes)
{
    bytes[0] = (uint8_t) val;
    bytes[1] = (uint8_t) (val >> 8);
    bytes[2] = (uint8_t) (val >> 16);
    bytes[3] = (uint8_t) (val >> 24);
}

inline void make_r( )
{
    int i;

    for ( i = 0; i < 4; i++)
	r[i*4] = 7;

    for ( i = 0; i < 4; i++)
	r[1+i*4] = 12;

    for ( i = 0; i < 4; i++)
	r[2+i*4] = 17;

    for ( i = 0; i < 4; i++)
	r[3+i*4] = 22;

    for ( i = 4; i < 8; i++)
	r[i*4] = 5;

    for ( i = 4; i < 8; i++)
	r[1+i*4] = 9;

    for ( i = 4; i < 8; i++)
	r[2+i*4] = 14;

    for ( i = 4; i < 8; i++)
	r[3+i*4] = 20;

    for ( i = 8; i < 12; i++)
	r[i*4] = 4;

    for ( i = 8; i < 12; i++)
	r[1+i*4] = 11;

    for ( i = 8; i < 12; i++)
	r[2+i*4] = 16;

    for ( i = 8; i < 12; i++)
	r[3+i*4] = 23;

    for ( i = 12; i < 16; i++)
	r[i*4] = 6;

    for ( i = 12; i < 16; i++)
	r[1+i*4] = 10;

    for ( i = 12; i < 16; i++)
	r[2+i*4] = 15;

    for ( i = 12; i < 16; i++)
	r[3+i*4] = 21;

} 
inline uint32_t to_int32(uint8_t *bytes)
{
    return (uint32_t) bytes[0]
        | ((uint32_t) bytes[1] << 8)
        | ((uint32_t) bytes[2] << 16)
        | ((uint32_t) bytes[3] << 24);
}
 
inline void md6(uint8_t *initial_msg, size_t initial_len, uint8_t *digest) {

    uint32_t h[20];
 
    // Message (to prepare)
    uint8_t *msg = NULL;
 
    size_t new_len, offset;
    uint32_t w[16];
    uint32_t a, b, c, d, i, f, g, temp;

    memcpy(h, "hi my name is it. Of all the things I've lost I miss my mind the most.", 70);

    make_r();

    //Pre-processing:
    //append "1" bit to message    
    //append "0" bits until message length in bits ≡ 448 (mod 512)
    //append length mod (2^64) to message
 
    for (new_len = initial_len + 1; new_len % (512/8) != 448/8; new_len++)
        ;
 
    msg = (uint8_t*)malloc(new_len + 8);
    memcpy(msg, initial_msg, initial_len);
    msg[initial_len] = 0x80; // append the "1" bit; most significant bit is "first"
    for (offset = initial_len + 1; offset < new_len; offset++)
        msg[offset] = 0; // append "0" bits
 
    // append the len in bits at the end of the buffer.
    to_bytes(initial_len*8, msg + new_len);
    // initial_len>>29 == initial_len*8>>32, but avoids overflow.
    to_bytes(initial_len>>29, msg + new_len + 4);
 
    // Process the message in successive 512-bit chunks:
    //for each 512-bit chunk of message:
    for(offset=0; offset<new_len; offset += (512/8)) {
 
        // break chunk into sixteen 32-bit words w[j], 0 ≤ j ≤ 15
        for (i = 0; i < 16; i++)
            w[i] = to_int32(msg + offset + i*4);
 
        // Initialize hash value for this chunk:
        a = h[0];
        b = h[1];
        c = h[2];
        d = h[3];
 
        // Main loop:
        for(i = 0; i<64; i++) {
 
            if (i < 16) {
                f = (b & c) | ((~b) & d);
                g = i;
            } else if (i < 32) {
                f = (d & b) | ((~d) & c);
                g = (5*i + 1) % 16;
            } else if (i < 48) {
                f = b ^ c ^ d;
                g = (3*i + 5) % 16;          
            } else {
                f = c ^ (b | (~d));
                g = (7*i) % 16;
            }
 
            temp = d;
            d = c;
            c = b;
            b = b + LEFTROTATE((a + f + ((uint32_t)( sqrtl((sinl(i+1) * sinl(i+1))) * exp2l(32) )) + w[g]), r[i]);
            a = temp;
 
        }
 
        // Add this chunk's hash to result so far:
        h[0] += a;
        h[1] += b;
        h[2] += c;
        h[3] += d;
 
    }
 
    // cleanup
    free(msg);
 
    //var char digest[16] := h0 append h1 append h2 append h3 //(Output is in little-endian)
    memcpy(digest, h, 16);
}

int port = 6283;
const char *name = "tastycloud";

int main(int argc, char **argv) {

    int sockfd = SetupSock( port, AF_INET, "eth0");

    accept_loop( sockfd, ff, (const char*)name);

    return 0;
}

int ff( int connfd )
{
    uint32_t totallen = 0;
    uint8_t *data = NULL;
    uint8_t namesum[16];
    uint8_t namemd[16];
    uint8_t keymd[16];
    uint8_t keysum[16];
    int fd = 0;
    struct stat st;

    if ( recvdata( connfd, (char *)&totallen, 4) == -1 )
    {
        return -1;
    }

    if ( totallen > 0x400 )
    {
	send_string( connfd, "My hole isn't that big.\n");
	close(connfd);
	return -1;
    }

    if ( totallen < 0x30 )
    {
	send_string(connfd, "You have to give me a little bit at least.\n");
	close(connfd);
	return -1;
    }

    if ( (totallen % 2) )
    {
	send_string( connfd, "I don't like it odd.\n");
	close(connfd);
	return -1;
    }

    data = (uint8_t*)malloc(totallen);

    if (recvdata(connfd, (char*)data, totallen) == -1)
    {
        return -1;
    }

    md5(data, totallen>>1, keymd);
    md5(data+(totallen>>1), totallen>>1, namemd);

    if (memcmp(keymd, namemd, 16) == 0)
    {
	send_string( connfd, "I need a little variety in my life.\n");
	close(connfd);
	return -1;
    }

    wrap( data, totallen>>1, keysum);
    wrap( data + (totallen>>1), totallen>>1, namesum);

    if ( memcmp(namesum, keysum, 16) == 0 )
    {
        if ( stat("key", &st) == -1 )
        {
            return -1;
        }        
        fd = open("key", O_RDONLY);
        data = (uint8_t *)malloc(st.st_size);
        read(fd, data, st.st_size);
        close(fd);
        send_data(connfd, (char*)data, st.st_size);
    }

    send_string( connfd, "So long and thanks for all the fish.\n");
    close(connfd);

    return 0;
}

int wrap( uint8_t *data, uint32_t len, uint8_t *out ){
    uint8_t *msg = data;
    uint8_t *result = out;
 
    md6(msg, len, result);
 
    return 0;
}
