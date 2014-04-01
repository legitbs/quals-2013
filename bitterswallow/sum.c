#include "common.h"

/* I wrote this do as you please */
void sum8( const unsigned char *input, size_t ilen, unsigned char output[1] )
{
    uint8_t s = 0;
    uint8_t c = 0;

    while ( c < ilen)
    {
       s += input[c++];
    }

    memcpy(output, &s, 1);

    return;
}

void sum16( const unsigned char *input, size_t ilen, unsigned char output[2] )
{
    uint16_t s = 0;

    while ( ilen > 1 )
    {
        s = s + ((uint16_t*)input)[0];
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
void sum32( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
    uint32_t s = 0;

    while ( ilen > 3 )
    {
        s = s + ((uint32_t*)input)[0];
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
