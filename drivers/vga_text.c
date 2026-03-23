/* drivers/vga_text.c
 * VGA text mode for debugging
 */

#include <stdint.h>
#include "../kernel/io.h"

#define VGA_ADDR 0xB8000
#define COLS 80
#define ROWS 25

static uint16_t* vga = (uint16_t*)0xB8000;
static int cx = 0, cy = 0;

void vga_clear(void) {
    for (int i = 0; i < COLS * ROWS; i++) {
        vga[i] = 0x0700;
    }
    cx = 0; cy = 0;
}

void vga_putc(char c) {
    if (c == '\n') {
        cx = 0; cy++;
    } else if (c == '\r') {
        cx = 0;
    } else {
        vga[cy * COLS + cx] = 0x0F00 | c;
        cx++;
    }
    if (cx >= COLS) { cx = 0; cy++; }
    if (cy >= ROWS) cy = ROWS - 1;
    
    uint16_t pos = cy * COLS + cx;
    outb(0x3D4, 0x0F);
    outb(0x3D5, pos & 0xFF);
    outb(0x3D4, 0x0E);
    outb(0x3D5, (pos >> 8) & 0xFF);
}

void vga_puts(const char* s) {
    while (*s) vga_putc(*s++);
}

void vga_puthex(uint32_t n) {
    vga_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        int d = (n >> i) & 0xF;
        vga_putc(d < 10 ? '0' + d : 'A' + d - 10);
    }
}
