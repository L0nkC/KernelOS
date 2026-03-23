/* drivers/vga13h.c
 * VGA Mode 13h - 320x200 256 colors
 * Simple, reliable, works everywhere
 */

#include <stdint.h>
#include "../kernel/io.h"
#include "vga13h.h"

#define FB_ADDR 0xA0000
#define WIDTH 320
#define HEIGHT 200

static uint8_t* fb = (uint8_t*)FB_ADDR;

void vga13h_init(void) {
    /* Set Mode 13h via BIOS would normally happen here, but we can't use BIOS.
     * Instead, we'll use the fact that QEMU starts in Mode 13h compatible mode
     * or the user can set it manually.
     * 
     * For a real OS, we'd program the VGA registers directly.
     * For now, we'll just clear the screen and assume mode 13h.
     */
    
    /* Clear screen to dark blue */
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        fb[i] = 1;  /* Color 1 = blue in default palette */
    }
}

void vga13h_clear(uint8_t color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        fb[i] = color;
    }
}

void vga13h_pixel(int x, int y, uint8_t color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    fb[y * WIDTH + x] = color;
}

void vga13h_fill_rect(int x, int y, int w, int h, uint8_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > WIDTH) w = WIDTH - x;
    if (y + h > HEIGHT) h = HEIGHT - y;
    if (w <= 0 || h <= 0) return;
    
    for (int row = y; row < y + h; row++) {
        uint8_t* p = fb + row * WIDTH + x;
        for (int col = 0; col < w; col++) {
            *p++ = color;
        }
    }
}

void vga13h_rect(int x, int y, int w, int h, uint8_t color) {
    for (int i = 0; i < w; i++) {
        vga13h_pixel(x + i, y, color);
        vga13h_pixel(x + i, y + h - 1, color);
    }
    for (int i = 0; i < h; i++) {
        vga13h_pixel(x, y + i, color);
        vga13h_pixel(x + w - 1, y + i, color);
    }
}

uint32_t vga13h_width(void) { return WIDTH; }
uint32_t vga13h_height(void) { return HEIGHT; }
