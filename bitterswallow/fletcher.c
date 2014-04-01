#include "common.h"

/* Based upon wikipedia en.wikipedia.org/wiki/Fletcher's_checksum. I didn't see a license */
void fletcher16( const unsigned char *input, size_t ilen, unsigned char output[2] )
{
    uint16_t sum1 = 0;
    uint16_t sum2 = 0;
    uint16_t c = 0;

    while (c < ilen)
    {
        sum1 = (sum1 + input[c++]) % 0xff;
        sum2 = (sum2 + sum1) % 0xff;
    }

    sum2 = (sum2 <<8) | sum1;

    memcpy(output, &sum2, 2);

    return;
}


void fletcher32( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
    uint32_t sum1 = 0xffff;
    uint32_t sum2 = 0xffff;

    while (ilen > 1)
    {
        sum1 = (sum1 + ((uint16_t*)input)[0]);
        input += 2;
        ilen -= 2;
        sum2 = (sum2 + sum1);
        sum1 = (sum1 & 0xffff) + (sum1 >> 16);
        sum2 = (sum2 & 0xffff) + (sum2 >> 16);
    }

    
    if ( ilen > 0 )
    {
        sum1 = (sum1 + input[0]) % 0xffff;
        sum2 = (sum1 + sum2) % 0xffff;
    }
    
    sum1 = (sum1 & 0xffff) + (sum1 >> 16);
    sum2 = (sum2 & 0xffff) + (sum2 >> 16);

    sum2 = (sum2<<16) | sum1;
    memcpy(output, &sum2, 4);

    return;
}
