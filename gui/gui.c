/* gui/gui.c */
#include <stdint.h>
#include "../kernel/io.h"

#define FB 0xA0000
#define W 320
#define H 200

static uint8_t* fb = (uint8_t*)FB;

static void set_vga_mode13h(void) {
    /* Set VGA Mode 13h (320x200x256) via BIOS would be ideal,
     * but we can also program the VGA registers directly.
     * For now, let's just clear the text screen and use it. */
}

static void rect(int x, int y, int w, int h, uint8_t c) {
    for (int row = y; row < y + h; row++) {
        for (int col = x; col < x + w; col++) {
            if (row >= 0 && row < H && col >= 0 && col < W) {
                fb[row * W + col] = c;
            }
        }
    }
}

static void border(int x, int y, int w, int h, uint8_t c) {
    for (int i = 0; i < w; i++) {
        if (x + i >= 0 && x + i < W) {
            if (y >= 0 && y < H) fb[y * W + x + i] = c;
            if (y + h - 1 >= 0 && y + h - 1 < H) fb[(y + h - 1) * W + x + i] = c;
        }
    }
    for (int i = 0; i < h; i++) {
        if (y + i >= 0 && y + i < H) {
            if (x >= 0 && x < W) fb[(y + i) * W + x] = c;
            if (x + w - 1 >= 0 && x + w - 1 < W) fb[(y + i) * W + x + w - 1] = c;
        }
    }
}

static void win(int x, int y, int w, int h, uint8_t t, uint8_t bg) {
    rect(x + 2, y + 2, w, h, 0);
    rect(x, y, w, h, bg);
    rect(x, y, w, 10, t);
    border(x, y, w, h, 15);
}

void gui_run(void) {
    /* Switch to Mode 13h by writing to VGA registers */
    /* Attribute Controller - disable video first */
    inb(0x3DA);  /* Reset attribute flip-flop */
    outb(0x3C0, 0x00);  /* Disable video */
    
    /* Sequencer registers */
    outb(0x3C4, 0x01); outb(0x3C5, 0x01);  /* Clock mode */
    outb(0x3C4, 0x04); outb(0x3C5, 0x0E);  /* Memory mode - chain 4 */
    
    /* Graphics Controller */
    outb(0x3CE, 0x05); outb(0x3CF, 0x40);  /* Mode register */
    outb(0x3CE, 0x06); outb(0x3CF, 0x05);  /* Misc register - A0000, 64KB, graphics */
    
    /* CRTC registers */
    outb(0x3D4, 0x11); uint8_t v = inb(0x3D5) & 0x7F; outb(0x3D4, 0x11); outb(0x3D5, v);
    outb(0x3D4, 0x00); outb(0x3D5, 0x5F);
    outb(0x3D4, 0x01); outb(0x3D5, 0x4F);
    outb(0x3D4, 0x02); outb(0x3D5, 0x50);
    outb(0x3D4, 0x03); outb(0x3D5, 0x82);
    outb(0x3D4, 0x04); outb(0x3D5, 0x54);
    outb(0x3D4, 0x05); outb(0x3D5, 0x80);
    outb(0x3D4, 0x06); outb(0x3D5, 0xBF);
    outb(0x3D4, 0x07); outb(0x3D5, 0x1F);
    outb(0x3D4, 0x08); outb(0x3D5, 0x00);
    outb(0x3D4, 0x09); outb(0x3D5, 0x41);
    outb(0x3D4, 0x0A); outb(0x3D5, 0x00);
    outb(0x3D4, 0x0B); outb(0x3D5, 0x00);
    outb(0x3D4, 0x0C); outb(0x3D5, 0x00);
    outb(0x3D4, 0x0D); outb(0x3D5, 0x00);
    outb(0x3D4, 0x0E); outb(0x3D5, 0x00);
    outb(0x3D4, 0x0F); outb(0x3D5, 0x00);
    outb(0x3D4, 0x10); outb(0x3D5, 0x9C);
    outb(0x3D4, 0x11); outb(0x3D5, 0x8E);
    outb(0x3D4, 0x12); outb(0x3D5, 0x8F);
    outb(0x3D4, 0x13); outb(0x3D5, 0x28);  /* Offset - 40 bytes per row (320 pixels / 8) */
    outb(0x3D4, 0x14); outb(0x3D5, 0x40);
    outb(0x3D4, 0x15); outb(0x3D5, 0x96);
    outb(0x3D4, 0x16); outb(0x3D5, 0xB9);
    outb(0x3D4, 0x17); outb(0x3D5, 0xA3);
    outb(0x3D4, 0x18); outb(0x3D5, 0xFF);
    
    /* Re-enable video */
    inb(0x3DA);
    outb(0x3C0, 0x20);  /* Enable video */
    
    /* Clear to dark gray */
    for (int i = 0; i < W * H; i++) fb[i] = 8;
    
    /* Top bar */
    rect(0, 0, W, 12, 1);
    
    /* Windows */
    win(20, 20, 80, 50, 9, 7);
    win(120, 30, 80, 50, 2, 7);
    win(50, 100, 80, 50, 4, 7);
    win(180, 90, 80, 50, 5, 7);
    
    while (1) __asm__ volatile ("hlt");
}
