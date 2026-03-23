/* drivers/vesa.h */
#ifndef VESA_H
#define VESA_H

#include <stdint.h>

#define RGB(r,g,b)      ((0xFF << 24) | ((r) << 16) | ((g) << 8) | (b))
#define CLR_BLACK       0xFF000000
#define CLR_WHITE       0xFFFFFFFF
#define CLR_RED         0xFFFF0000
#define CLR_GREEN       0xFF00FF00
#define CLR_BLUE        0xFF0000FF
#define CLR_YELLOW      0xFFFFFF00
#define CLR_CYAN        0xFF00FFFF
#define CLR_MAGENTA     0xFFFF00FF
#define CLR_GRAY        0xFF808080
#define CLR_DKGRAY      0xFF404040
#define CLR_LTGRAY      0xFFC0C0C0
#define UI_BG           0xFF2C3E50
#define UI_PANEL        0xFF34495E
#define UI_ACCENT       0xFF3498DB
#define UI_WINDOW_BG    0xFFECF0F1

int vesa_init(void);
void vesa_exit(void);
void vesa_clear(uint32_t color);
void vesa_pixel(int x, int y, uint32_t color);
void vesa_fill_rect(int x, int y, int w, int h, uint32_t color);
void vesa_rect(int x, int y, int w, int h, uint32_t color);
void vesa_flip(void);
uint32_t vesa_width(void);
uint32_t vesa_height(void);

#endif
