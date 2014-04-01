#include "common.h"

/* I wrote this file and I don't care where it goes */

void cos128( const unsigned char *input, size_t ilen, unsigned char output[16])
{
    size_t i = ilen / 4;
    long double j = 0;

    while (i--)
    {
        j += cosl(((long double *)input)[i]);
    }

    memcpy( output, &j, 16);

    return;
}

void acos128( const unsigned char *input, size_t ilen, unsigned char output[16])
{
    size_t i = ilen / 4;
    long double j = 0;

    while (i--)
    {
        j += acosl(((long double *)input)[i]);
    }

    memcpy( output, &j, 16);

    return;
}
