/* drivers/vesa.c
 * VBE 2.0 / Bochs VBE Graphics
 * 800x600 32bpp with double buffering
 */

#include <stdint.h>
#include "../kernel/io.h"
#include "vesa.h"

#define VBE_PORT_INDEX  0x01CE
#define VBE_PORT_DATA   0x01CF
#define VBE_REG_ENABLE  0x04
#define VBE_REG_XRES    0x01
#define VBE_REG_YRES    0x02
#define VBE_REG_BPP     0x03
#define VBE_ENABLED     0x01
#define VBE_LFB_ENABLED 0x40
#define VBE_FB_ADDR     0xE0000000

#define WIDTH   800
#define HEIGHT  600

/* Back buffer in low memory (identity mapped) */
static uint32_t back_buffer[WIDTH * HEIGHT] __attribute__((aligned(4096)));
static uint32_t* front_buffer = (uint32_t*)VBE_FB_ADDR;

static inline void vbe_write(uint16_t reg, uint16_t val) {
    outw(VBE_PORT_INDEX, reg);
    outw(VBE_PORT_DATA, val);
}

static inline uint16_t vbe_read(uint16_t reg) {
    outw(VBE_PORT_INDEX, reg);
    return inw(VBE_PORT_DATA);
}

int vesa_init(void) {
    uint16_t id = vbe_read(0);
    if (id < 0xB0C0) return -1;
    
    vbe_write(VBE_REG_ENABLE, 0);
    vbe_write(VBE_REG_XRES, WIDTH);
    vbe_write(VBE_REG_YRES, HEIGHT);
    vbe_write(VBE_REG_BPP, 32);
    vbe_write(VBE_REG_ENABLE, VBE_ENABLED | VBE_LFB_ENABLED);
    
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        back_buffer[i] = 0xFF2C3E50;
        front_buffer[i] = 0xFF2C3E50;
    }
    
    return 0;
}

void vesa_exit(void) {
    vbe_write(VBE_REG_ENABLE, 0);
}

void vesa_clear(uint32_t color) {
    for (int i = 0; i < WIDTH * HEIGHT; i++) back_buffer[i] = color;
}

void vesa_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    back_buffer[y * WIDTH + x] = color;
}

void vesa_fill_rect(int x, int y, int w, int h, uint32_t color) {
    if (x < 0) { w += x; x = 0; }
    if (y < 0) { h += y; y = 0; }
    if (x + w > WIDTH) w = WIDTH - x;
    if (y + h > HEIGHT) h = HEIGHT - y;
    if (w <= 0 || h <= 0) return;
    
    for (int row = y; row < y + h; row++) {
        uint32_t* p = back_buffer + row * WIDTH + x;
        for (int col = 0; col < w; col++) *p++ = color;
    }
}

void vesa_rect(int x, int y, int w, int h, uint32_t color) {
    for (int i = 0; i < w; i++) {
        vesa_pixel(x + i, y, color);
        vesa_pixel(x + i, y + h - 1, color);
    }
    for (int i = 0; i < h; i++) {
        vesa_pixel(x, y + i, color);
        vesa_pixel(x + w - 1, y + i, color);
    }
}

void vesa_flip(void) {
    uint32_t* src = back_buffer;
    uint32_t* dst = front_buffer;
    int count = WIDTH * HEIGHT;
    while (count >= 4) {
        dst[0] = src[0]; dst[1] = src[1];
        dst[2] = src[2]; dst[3] = src[3];
        dst += 4; src += 4; count -= 4;
    }
    while (count--) *dst++ = *src++;
}

uint32_t vesa_width(void) { return WIDTH; }
uint32_t vesa_height(void) { return HEIGHT; }
