#include "hjalloc.h"

pheap create_heap( uint64_t size, uint64_t flags )
{
    pheap new_heap = NULL;

    if ( size < 0x4000 )
    {
        return new_heap;
    }

    new_heap = (pheap)malloc( sizeof(heap) );

    if ( new_heap == NULL )
    {
        return new_heap;
    }

    memset( new_heap, 0x00, sizeof(heap) );

    new_heap->heap_size = size;
    new_heap->flags = flags;

    new_heap->heap_base_addr = (uint8_t *)malloc( size );

    if ( new_heap->heap_base_addr == NULL )
    {
        free( new_heap );
        return NULL;
    }

    // Add the initial block to the heap
    if ( free_fl_block( new_heap, new_heap->heap_base_addr, size) != 0 )
    {
        free( new_heap->heap_base_addr );
        free( new_heap );
        return NULL;
    }

    return new_heap;
}
