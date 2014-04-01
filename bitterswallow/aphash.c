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

void aphash(const unsigned char* input, size_t ilen, unsigned char output[4])
{
    unsigned int hash = 0xAAAAAAAA;
    unsigned int i    = 0;

    for(i = 0; i < ilen; input++, i++)
    {
        hash ^= ((i & 1) == 0) ? (  (hash <<  7) ^ (*input) * (hash >> 3)) :
                               (~((hash << 11) + ((*input) ^ (hash >> 5))));
    }

    memcpy( output, &hash, 4);

    return;
}
