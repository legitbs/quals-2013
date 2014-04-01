#include "common.h"

/* I wrote this do as you please */
void tan128( const unsigned char *input, size_t ilen, unsigned char output[16])
{
    size_t i = ilen / 4;
    long double j = 0;

    while (i--)
    {
        j += tanl(((long double *)input)[i]);
    }

    memcpy( output, &j, 16);

    return;
}
void atan128( const unsigned char *input, size_t ilen, unsigned char output[16])
{
    size_t i = ilen / 4;
    long double j = 0;

    while (i--)
    {
        j += atanl(((long double *)input)[i]);
    }

    memcpy( output, &j, 16);

    return;
}
