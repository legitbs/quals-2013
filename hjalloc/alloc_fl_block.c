#include "hjalloc.h"

void *alloc_fl_block( pheap heap, uint64_t size )
{
    uint64_t remaining = 0;
    void *alloc_block = NULL;
    pfl walker = NULL;

    if ( heap == NULL )
    {
        return NULL;
    }

    if ( size == 0 )
    {
        return NULL;
    }

    walker = (pfl)(heap->lal[0]);

    // Handle the case where no blocks are large enough
    if ( walker->prev->block_size < size )
    {
        return NULL;
    }

    while ( walker->block_size < size )
    {
        walker = walker->next;
    }

    // safely unlink
    if ( (walker->prev->next != walker) || (walker->next->prev != walker) )    
    {
        exit(-1);
    }

    remaining = walker->block_size - size;
    alloc_block = (void*)(( (uint8_t*)(walker) )+remaining);
    walker->block_size = remaining;

    // Unlink the walker block
    // Handle the single block case
    if ( heap->blocks_in_fl == 1 )
    {
        heap->lal[0] = NULL;
    } else
    {
        // Update the head pointer
        if ( heap->lal[0] == (uint8_t*)walker )
        {
            heap->lal[0] = (uint8_t*)walker->next;
        }

        // Unlink normally
        walker->prev->next = walker->next;
        walker->next->prev = walker->prev;
    }
    
    heap->blocks_in_fl--;

    free_fl_block( heap, (void*)walker, walker->block_size);

    return alloc_block;
}
