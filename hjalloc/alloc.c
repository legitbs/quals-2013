#include "hjalloc.h"

void *hjalloc( pheap heap, uint64_t size )
{
    void *block = 0;

    if ( heap == NULL )
    {
        return NULL;
    }

    if ( size == 0 )
    {
        return NULL;
    }

    // Align size to next highest 16=byte block
    size = (size + 15);
    size >>= 4;
    size <<= 4;
    
    if ( size <= 1024 )
    {
        block = (void*)(alloc_lal_block( heap, size ));

        if ( block )
        {
            return block;
        }
    }

    block = (void*)(alloc_fl_block( heap, size ));

    return block;
}
