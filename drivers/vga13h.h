/* drivers/vga13h.h */
#ifndef VGA13H_H
#define VGA13H_H

#include <stdint.h>

/* Mode 13h palette colors */
#define C_BLACK     0
#define C_BLUE      1
#define C_GREEN     2
#define C_CYAN      3
#define C_RED       4
#define C_MAGENTA   5
#define C_BROWN     6
#define C_LGRAY     7
#define C_DGRAY     8
#define C_LBLUE     9
#define C_LGREEN    10
#define C_LCYAN     11
#define C_LRED      12
#define C_LMAGENTA  13
#define C_YELLOW    14
#define C_WHITE     15

void vga13h_init(void);
void vga13h_clear(uint8_t color);
void vga13h_pixel(int x, int y, uint8_t color);
void vga13h_fill_rect(int x, int y, int w, int h, uint8_t color);
void vga13h_rect(int x, int y, int w, int h, uint8_t color);
uint32_t vga13h_width(void);
uint32_t vga13h_height(void);

#endif
