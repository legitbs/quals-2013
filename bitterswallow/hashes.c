#include "common.h"

/*
 **************************************************************************
 *                                                                        *
 *          General Purpose Hash Function Algorithms Library              *
 *                                                                        *
 * Author: Arash Partow - 2002                                            *
 * URL: http://www.partow.net                                             *
 * URL: http://www.partow.net/programming/hashfunctions/index.html        *
 *                                                                        *
 * Copyright notice:                                                      *
 * Free use of the General Purpose Hash Function Algorithms Library is    *
 * permitted under the guidelines and in accordance with the most current *
 * version of the Common Public License.                                  *
 * http://www.opensource.org/licenses/cpl1.0.php                          *
 *                                                                        *
 **************************************************************************
*/

void jshash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
    unsigned int hash = 1315423911;
    unsigned int i    = 0;

    for(i = 0; i < ilen; input++, i++)
    {
        hash ^= ((hash << 5) + (*input) + (hash >> 2));
    }

    memcpy( output, &hash, 4);
    return;
}

void pjwhash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   const unsigned int BitsInUnsignedInt = (unsigned int)(sizeof(unsigned int) * 8);
   const unsigned int ThreeQuarters     = (unsigned int)((BitsInUnsignedInt  * 3) / 4);
   const unsigned int OneEighth         = (unsigned int)(BitsInUnsignedInt / 8);
   const unsigned int HighBits          = (unsigned int)(0xFFFFFFFF) << (BitsInUnsignedInt - OneEighth);
   unsigned int hash              = 0;
   unsigned int test              = 0;
   unsigned int i                 = 0;

   for(i = 0; i < ilen; input++, i++)
   {
      hash = (hash << OneEighth) + (*input);

      if((test = hash & HighBits)  != 0)
      {
         hash = (( hash ^ (test >> ThreeQuarters)) & (~HighBits));
      }
   }

    memcpy( output, &hash, 4);
   return;
}

void elfhash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   unsigned int hash = 0;
   unsigned int x    = 0;
   unsigned int i    = 0;

   for(i = 0; i < ilen; input++, i++)
   {
      hash = (hash << 4) + (*input);
      if((x = hash & 0xF0000000L) != 0)
      {
         hash ^= (x >> 24);
      }
      hash &= ~x;
   }

    memcpy( output, &hash, 4);
   return;
}

void bkdrhash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   unsigned int seed = 131; /* 31 131 1313 13131 131313 etc.. */
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < ilen; input++, i++)
   {
      hash = (hash * seed) + (*input);
   }

    memcpy( output, &hash, 4);
   return;
}

void sdbmhash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   unsigned int hash = 0;
   unsigned int i    = 0;

   for(i = 0; i < ilen; input++, i++)
   {
      hash = (*input) + (hash << 6) + (hash << 16) - hash;
   }

    memcpy( output, &hash, 4);
   return;
}

void djbhash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   unsigned int hash = 5381;
   unsigned int i    = 0;

   for(i = 0; i < ilen; input++, i++)
   {
      hash = ((hash << 5) + hash) + (*input);
   }

    memcpy( output, &hash, 4);
   return;
}

void dekhash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   unsigned int hash = ilen;
   unsigned int i    = 0;

   for(i = 0; i < ilen; input++, i++)
   {
      hash = ((hash << 5) ^ (hash >> 27)) ^ (*input);
   }

    memcpy( output, &hash, 4);
   return;
}

void bphash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   unsigned int hash = 0;
   unsigned int i    = 0;
   for(i = 0; i < ilen; input++, i++)
   {
      hash = hash << 7 ^ (*input);
   }

    memcpy(output, &hash, 4);
   return;
}

void fnvhash( const unsigned char *input, size_t ilen, unsigned char output[4] )
{
   const unsigned int fnv_prime = 0x811C9DC5;
   unsigned int hash      = 0;
   unsigned int i         = 0;

   for(i = 0; i < ilen; input++, i++)
   {
      hash *= fnv_prime;
      hash ^= (*input);
   }

    memcpy( output, &hash, 4);
   return;
}
