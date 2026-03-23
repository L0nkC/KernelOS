/* kernel/isr.c */
#include <stdint.h>

typedef struct { uint32_t ds, di, si, bp, sp, bx, dx, cx, ax, n, err, ip, cs, f; } regs_t;

void isr_handler(regs_t* r) {
    volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
    vga[0] = 0x4F00 | 'E'; vga[1] = 0x4F00 | 'R';
    vga[2] = 0x4F00 | 'R'; vga[3] = 0x4F00 | ':';
    while (1) __asm__ volatile ("cli; hlt");
}
