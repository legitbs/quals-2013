#include "common.h"

/* I wrote this and I don't care */
void mod8( const unsigned char *input, size_t ilen, unsigned char output[1] )
{
    uint8_t s = 0xde;
    uint8_t c = 0;

    while ( c < ilen)
    {
       s = s + (s % input[c++]);
    }

    memcpy(output, &s, 1);

    return;
}

void mod16( const unsigned char *input, size_t ilen, unsigned char output[2] )
{
    uint16_t s = 0xdead;

    while ( ilen > 1 )
    {
        s = s + (s % ((uint16_t*)input)[0]);
        input += 2;
        ilen -= 2;
    }

    if ( ilen > 0 )
    {
        s += *((uint8_t*)input);
    }

    memcpy(output, &s, 2);

    return;
}
void mod32( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
    uint32_t s = 0xdeadbabe;

    while ( ilen > 3 )
    {
        s = s + (s % ((uint32_t*)input)[0]);
        input += 4;
        ilen -= 4;
    }

    if ( ilen > 0)
    {
        s += (*((uint32_t*)input) & 0xFFFFFF);
    }

    memcpy(output, &s, 4);

    return;
}
