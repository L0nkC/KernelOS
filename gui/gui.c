/* gui/gui.c - Draw windows in Mode 13h */
#include <stdint.h>

#define FB 0xA0000
#define W 320
#define H 200

static void rect(int x, int y, int w, int h, uint8_t c) {
    uint8_t* fb = (uint8_t*)FB;
    for (int row = y; row < y + h && row < H; row++) {
        for (int col = x; col < x + w && col < W; col++) {
            if (row >= 0 && col >= 0) {
                fb[row * W + col] = c;
            }
        }
    }
}

static void win(int x, int y, int w, int h, uint8_t title, uint8_t bg) {
    /* Shadow */
    rect(x + 2, y + 2, w, h, 0);
    /* Body */
    rect(x, y, w, h, bg);
    /* Title bar */
    rect(x, y, w, 10, title);
}

void gui_run(void) {
    /* Clear desktop */
    rect(0, 0, W, H, 8);
    
    /* Top bar */
    rect(0, 0, W, 12, 1);
    
    /* Windows */
    win(20, 20, 80, 50, 9, 7);
    win(120, 30, 80, 50, 2, 7);
    win(50, 100, 80, 50, 4, 7);
    win(180, 90, 80, 50, 5, 7);
}
