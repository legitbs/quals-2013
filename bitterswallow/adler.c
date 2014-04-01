#include "common.h"

/* Adler32 from Wikipedia article en.wikipedia.org/wiki/Adler-32.
 * I belive I just copied the code for adler16 and adler8 */

const int MOD_ADLER32 = 65521;
const int MOD_ADLER16 = 0xFB;
const int MOD_ADLER8 = 0xD;

void adler32(const unsigned char *input, size_t ilen, unsigned char output[4])
{
    uint32_t a = 1, b = 0;
    int32_t index;
 
    /* Process each byte of the data in order */
    for (index = 0; index < ilen; ++index)
    {
        a = (a + input[index]) % MOD_ADLER32;
        b = (b + a) % MOD_ADLER32;
    }
 
    b = (b << 16) | a;

    memcpy( output, &b, 4);

    return;
}

void adler16(const unsigned char *input, size_t ilen, unsigned char output[2])
{
    uint32_t a = 1, b = 0;
    int32_t index;

    /* Process each byte of the data in order */
    for (index = 0; index < ilen; ++index)
    {
        a = (a + input[index]) % MOD_ADLER16;
        b = (b + a) % MOD_ADLER16;
    }

    b = (b << 8) | a;

    memcpy( output, &b, 2);

    return;
}

void adler8(const unsigned char *input, size_t ilen, unsigned char output[8])
{
    uint32_t a = 1, b = 0;
    int32_t index;

    /* Process each byte of the data in order */
    for (index = 0; index < ilen; ++index)
    {
        a = (a + input[index]) % MOD_ADLER8;
        b = (b + a) % MOD_ADLER8;
    }

    b = (b << 4) | a;

    memcpy( output, &b, 1);

    return;
}

