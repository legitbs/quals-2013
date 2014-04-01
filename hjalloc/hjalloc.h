#ifndef __HJALLOC_H__
#define __HJALLOC_H__

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#define HEAP_FLAG_BUSY 0x01
#define HEAP_FLAG_FREE 0x10
#define HEAP_FLAG_NO_COALESCE 0x100

typedef struct HeapMeta_t
{
    uint64_t heap_size;
    uint64_t total_free;
    uint8_t *heap_base_addr;
    uint64_t blocks_in_fl;
    void *lal[65];
    uint64_t flags;
} heap, *pheap;

typedef struct FreeListMeta_t
{
    uint64_t block_size;
    struct FreeListMeta_t *next;
    struct FreeListMeta_t *prev;
    uint64_t flags;
} fl, *pfl;

typedef struct LalMeta_t
{
    struct LalMeta_t *next;
    uint64_t flags;
} lal, *plal;

uint32_t hjfree( pheap heap, void *block, uint64_t size );
uint32_t free_lal_block( pheap heap, void * block, uint64_t size);
void *alloc_lal_block( pheap heap, uint64_t size);
void *hjalloc( pheap heap, uint64_t size );
uint32_t print_fl_blocks( pheap heap );
uint32_t print_lal_blocks( pheap heap );
uint32_t free_fl_block( pheap heap, void *newblock, uint64_t size );
void *alloc_fl_block( pheap heap, uint64_t size );
pheap create_heap( uint64_t size, uint64_t flags );

uint32_t print_fl_blocks( pheap heap );
uint32_t print_lal_blocks( pheap heap );
#endif
