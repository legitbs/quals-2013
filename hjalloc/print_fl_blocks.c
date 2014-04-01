#include "hjalloc.h"

uint32_t print_fl_blocks( pheap heap )
{
    pfl walker = NULL;
    pfl end = NULL;

    if (heap == NULL)
    {
        return -1;
    }

    walker = (pfl)(heap->lal[0]);
    end = walker;

    do
    {
        printf("Addr: %lx Size: %lx Prev: %lx Next: %lx\n", (uint64_t)walker, (uint64_t)(walker->block_size), (uint64_t)(walker->prev), (uint64_t)(walker->next));
        walker = walker->next;
    } while( walker != end ); 

    return 1;   

}
