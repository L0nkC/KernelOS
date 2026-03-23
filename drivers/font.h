/* drivers/font.h */
#ifndef FONT_H
#define FONT_H

#include <stdint.h>

void font_draw_char(int x, int y, char c, uint32_t color);
void font_draw_string(int x, int y, const char* s, uint32_t color);
int font_string_width(const char* s);

#define FONT_W 8
#define FONT_H 16

#endif
