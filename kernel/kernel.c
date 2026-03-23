/* kernel/kernel.c
 * KernelOS - No paging test
 */

#include <stdint.h>
#include "../kernel/io.h"

__attribute__((section(".bss"), aligned(16)))
static uint8_t kstack[16384];

extern void gdt_init(void);

void kernel_main(uint32_t magic, void* mbi) {
    __asm__ volatile ("mov %0, %%esp" : : "r"(kstack + sizeof(kstack)));
    
    /* Direct VGA text write - no drivers */
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    
    /* Clear screen */
    for (int i = 0; i < 80 * 25; i++) {
        vga[i] = 0x0700 | ' ';
    }
    
    /* Write "OK" */
    vga[0] = 0x0F00 | 'O';
    vga[1] = 0x0F00 | 'K';
    
    while (1) __asm__ volatile ("cli; hlt");
}
