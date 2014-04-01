#include "common.h"

/* I wrote this do as you please */
void xor8( const unsigned char *input, size_t ilen, unsigned char output[1] )
{
    uint8_t s = 0xff;
    uint32_t c = 0;

    while (c < ilen)
    {
        s = s ^ input[c++];
    }

    memcpy( output, &s, 1);

    return;
}

void xor16( const unsigned char *input, size_t ilen, unsigned char output[2] )
{
    uint16_t s = 0xffff;
    uint32_t c = 0;

    while ( c < ilen )
    {
        s = s ^ ((uint16_t *)input)[0];
        input += 2;
        c += 2;
    }

    memcpy( output, &s, 2);

    return;
}

void xor32( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
    uint32_t s = 0xffffffff;
    uint32_t c = 0;

    while ( c < ilen)
    {
        s = s ^ ((uint32_t*)input)[0];
        input += 4;
        c += 4;
    }

    memcpy( output, &s, 4);

    return;
}
