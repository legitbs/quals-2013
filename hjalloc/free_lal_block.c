#include "hjalloc.h"

uint32_t free_lal_block( pheap heap, void *block, uint64_t size )
{
    uint64_t index = 0;
    plal l = NULL;

    if (heap == NULL)
    {
        return -1;
    }

    if ( block == NULL )
    {
        return -1;
    }

    if (size == 0 )
    {
        return -1;
    }

    if ( size > 1024 )
    {
        return -1;
    }

    size += 15;
    size >>=4;
    size <<=4;

    index = size>>4;

    l = (plal)block;
    l->next = (plal)heap->lal[index];   
    heap->lal[index] = (uint8_t*)block;

    return 0;
}
