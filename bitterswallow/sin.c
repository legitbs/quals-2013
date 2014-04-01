#include "common.h"

/* I wrote this do as you please */

void sin128( const unsigned char *input, size_t ilen, unsigned char output[16])
{
    size_t i = ilen / 4;
    long double j = 0;

    while (i--)
    {
        j += sinl(((long double *)input)[i]);
    }

    memcpy( output, &j, 16);

    return;
}
void asin128( const unsigned char *input, size_t ilen, unsigned char output[16])
{
    size_t i = ilen / 4;
    long double j = 0;

    while (i--)
    {
        j += asinl(((long double *)input)[i]);
    }

    memcpy( output, &j, 16);

    return;
}
