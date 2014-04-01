#include "hjalloc.h"

int main(int argc, char **argv)
{
    uint8_t *h = NULL;
    uint8_t *i;
    uint8_t *j;
    uint8_t *k;
    uint8_t *l;
    uint8_t *m;
    pheap myheap = create_heap( 0x4000, 0x0000);

    printf("-------------------------\n");
    print_fl_blocks( myheap );
    h = hjalloc( myheap, 0x100 );
    printf("-------------------------\n");
    print_fl_blocks( myheap );
    k = hjalloc( myheap, 0x100 );
    l = hjalloc( myheap, 0x100 );
    m = hjalloc( myheap, 0x100 );
    printf("-------------------------\n");
    hjfree(myheap, m, 0x100);
    hjfree(myheap, k, 0x100);
    hjfree(myheap, l, 0x100);
    print_fl_blocks( myheap );
    j = hjalloc( myheap, 0x2000);
    printf("-------------------------\n");
    print_fl_blocks( myheap );
    hjfree( myheap, h, 0x100 );
    printf("-------------------------\n");
    print_lal_blocks( myheap );
    h = hjalloc( myheap, 0x83 );
    printf("-------------------------\n");
    print_lal_blocks( myheap );
    k = hjalloc( myheap, 0x80);
    printf("-------------------------\n");
    print_lal_blocks( myheap );
    hjfree( myheap, h, 0x83 );
    printf("-------------------------\n");
    print_lal_blocks( myheap );
    hjfree( myheap, k, 0x80 );
    printf("-------------------------\n");
    print_lal_blocks( myheap );
    hjfree( myheap, j, 0x2000);
    printf("-------------------------\n");

    print_lal_blocks( myheap );
    print_fl_blocks( myheap );
    return 0;
}
