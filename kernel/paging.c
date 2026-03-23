/* kernel/paging.c
 * Paging with VBE framebuffer support
 */

#include <stdint.h>

static uint32_t page_directory[1024] __attribute__((aligned(4096)));
static uint32_t page_table0[1024] __attribute__((aligned(4096)));
static uint32_t page_table1[1024] __attribute__((aligned(4096)));
static uint32_t page_table2[1024] __attribute__((aligned(4096)));
static uint32_t page_table3[1024] __attribute__((aligned(4096)));
static uint32_t page_table_vbe[1024] __attribute__((aligned(4096)));

extern void enable_paging(uint32_t*);

void paging_init(void) {
    /* Identity map 0-16MB */
    for (int i = 0; i < 1024; i++) {
        page_table0[i] = (i * 4096) | 3;
        page_table1[i] = ((i + 1024) * 4096) | 3;
        page_table2[i] = ((i + 2048) * 4096) | 3;
        page_table3[i] = ((i + 3072) * 4096) | 3;
    }
    
    /* Map VBE framebuffer at 0xE0000000 (4MB) */
    uint32_t fb_base = 0xE0000000;
    for (int i = 0; i < 1024; i++) {
        page_table_vbe[i] = (fb_base + i * 4096) | 3;
    }
    
    page_directory[0] = ((uint32_t)page_table0) | 3;
    page_directory[1] = ((uint32_t)page_table1) | 3;
    page_directory[2] = ((uint32_t)page_table2) | 3;
    page_directory[3] = ((uint32_t)page_table3) | 3;
    
    /* Page dir entry 896 = 0xE0000000 >> 22 */
    page_directory[896] = ((uint32_t)page_table_vbe) | 3;
    
    for (int i = 4; i < 1024; i++) {
        if (i != 896) page_directory[i] = 0;
    }
    
    enable_paging(page_directory);
}
