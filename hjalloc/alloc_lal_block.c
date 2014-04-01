#include "hjalloc.h"

void *alloc_lal_block( pheap heap, uint64_t size )
{
    uint64_t index = 0;
    void *block = NULL;
    plal l = NULL;
    uint64_t extra = 0;

    if ( heap == NULL )
    {
        return NULL;
    }

    if ( size == 0 )
    {
        return NULL;
    }

    size += 15;
    size >>=4;
    size <<=4;

    if ( size > 1024 )
    {
        return NULL;
    }

    index = size>>4;

    while ( heap->lal[index] == NULL && index < 65 )
    {
        index++;
    }

    if ( index >= 65 )
    {
        return NULL;
    }

    l = (plal)(heap->lal[index]);

    // unlink
    heap->lal[index] = (uint8_t*)(l->next);

    memset( l, 0x00, index<<4);

    if ( size>>4 == index )
    {
        block = (uint8_t*)(l);
    } else
    {
        // set block
        extra = (index<<4)-size;
        block = (void*)(( (uint8_t*)(l)) + extra);

        // link in remaining block
        l->next = (plal)(heap->lal[extra>>4]);
        heap->lal[extra>>4] = (uint8_t*)(l);
    }

    heap->total_free -= size;

    return block;
}

