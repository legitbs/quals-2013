#include "hjalloc.h"

uint32_t hjfree( pheap heap, void *block, uint64_t size )
{
    if (heap == NULL)
    {
        return -1;
    }

    if ( block == NULL )
    {
        return -1;
    }

    if ( size == 0 )
    {
        return -1;
    }

    size += 15;
    size >>= 4;
    size <<= 4;

    if ( size <= 1024 )
    {
        free_lal_block( heap, block, size );
    } else
    {
        free_fl_block( heap, block, size);
    }

    return 0;
}
