/* kernel/paging.c */
#include <stdint.h>

static uint32_t pd[1024] __attribute__((aligned(4096)));
static uint32_t pt0[1024] __attribute__((aligned(4096)));
extern void enable_paging(uint32_t*);

void paging_init(void) {
    for (int i = 0; i < 1024; i++) pt0[i] = (i * 4096) | 3;
    pd[0] = ((uint32_t)pt0) | 3;
    for (int i = 1; i < 1024; i++) pd[i] = 0;
    enable_paging(pd);
}
