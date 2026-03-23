/* drivers/vga_text.h */
#ifndef VGA_TEXT_H
#define VGA_TEXT_H

#include <stdint.h>

void vga_clear(void);
void vga_putc(char c);
void vga_puts(const char* s);
void vga_puthex(uint32_t n);

#endif
