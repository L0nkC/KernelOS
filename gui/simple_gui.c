/* gui/simple_gui.c
 * Simple GUI for Mode 13h - No interrupts needed
 */

#include <stdint.h>
#include "../drivers/vga13h.h"
#include "../drivers/vga_text.h"

#define MAX_WIN 4

typedef struct {
    int x, y, w, h;
    uint8_t color;
    const char* title;
} win_t;

static win_t wins[MAX_WIN];
static int win_cnt = 0;

static void draw_win(win_t* w) {
    int x = w->x, y = w->y;
    int wt = w->w, h = w->h;
    
    /* Shadow */
    vga13h_fill_rect(x + 4, y + 4, wt, h, C_BLACK);
    
    /* Body */
    vga13h_fill_rect(x, y, wt, h, C_LGRAY);
    
    /* Title bar */
    vga13h_fill_rect(x, y, wt, 12, w->color);
    
    /* Border */
    vga13h_rect(x, y, wt, h, C_WHITE);
}

static void create_win(const char* t, int x, int y, int w, int h, uint8_t c) {
    if (win_cnt >= MAX_WIN) return;
    wins[win_cnt].x = x;
    wins[win_cnt].y = y;
    wins[win_cnt].w = w;
    wins[win_cnt].h = h;
    wins[win_cnt].color = c;
    wins[win_cnt].title = t;
    win_cnt++;
}

void simple_gui_run(void) {
    /* Create windows */
    create_win("Win1", 20, 20, 100, 60, C_BLUE);
    create_win("Win2", 140, 30, 100, 60, C_GREEN);
    create_win("Win3", 50, 100, 100, 60, C_RED);
    create_win("Win4", 180, 90, 100, 60, C_MAGENTA);
    
    /* Draw desktop */
    vga13h_clear(C_DGRAY);
    
    /* Draw panel */
    vga13h_fill_rect(0, 0, 320, 14, C_BLUE);
    
    /* Draw windows */
    for (int i = 0; i < win_cnt; i++) {
        draw_win(&wins[i]);
    }
    
    /* Draw some text using pixels (simple test pattern) */
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            vga13h_pixel(10 + x, 180 + y, C_WHITE);
        }
    }
    
    /* Done - just halt */
    vga_puts("\nGUI displayed. Halting.");
    while (1) {
        __asm__ volatile ("hlt");
    }
}
