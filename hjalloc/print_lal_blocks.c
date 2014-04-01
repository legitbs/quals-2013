#include "hjalloc.h"

uint32_t print_lal_blocks( pheap heap )
{
    uint64_t index = 1;
    plal walker = NULL;

    if ( heap == NULL )
    {
        return -1;
    }

    while ( index < 65 )
    {
        if ( heap->lal[index] != NULL )
        {
            printf("Index: %lx Size: %lx Flink: %lx", index, index<<4, (uint64_t)(heap->lal[index]));
            walker = ((plal)heap->lal[index])->next;

            while (walker)
            {
                printf(" --> %lx", (uint64_t)(walker));
                walker = walker->next;
            }
            printf("\n");
        }
        index++;
    }

    return 0;
}
