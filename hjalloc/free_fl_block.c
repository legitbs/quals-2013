#include "hjalloc.h"

uint32_t free_fl_block( pheap heap, void *newblock, uint64_t size )
{
    pfl walker = NULL;
    pfl end = NULL;
    pfl insert_block = NULL;

    if ( heap == NULL )
    {
        return -1;
    }

    if ( newblock == NULL )
    {
        return -1;
    }

    // Clear the newblock memory
    memset( newblock, 0x00, size );
    insert_block = (pfl)newblock;
    insert_block->block_size = size;
    heap->blocks_in_fl++;

    // If there are no blocks in the free list, this block starts it
    if ( heap->lal[0] == NULL )
    {
        insert_block->next = insert_block;
        insert_block->prev = insert_block;
        heap->total_free += insert_block->block_size;
        heap->lal[0] = (uint8_t*)insert_block;
        return 0;
    }

    // Locate the where the block needs to go
    walker = (pfl)heap->lal[0];
    end = walker;

    while( walker->block_size < size )
    {
        walker = walker->next;

        if ( walker == end )
        {
            break;
        }
    }

    // Put the block in its place
    insert_block->next = walker;
    insert_block->prev = walker->prev;
    walker->prev->next = insert_block;
    walker->prev = insert_block;
    
    // ensure the smallest is still at the head
    // since the only place that it could be inserted
    // is previous to the current head, I only need
    // to check the previous block size
    if ( ((pfl)heap->lal[0])->prev->block_size < ((pfl)heap->lal[0])->block_size )
    {
        heap->lal[0] = (uint8_t*)(((pfl)heap->lal[0])->prev);
    }
    
    return 0;
}
