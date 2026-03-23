/* gui/gui.c
 * Window Manager - Full GUI
 */

#include <stdint.h>
#include "../drivers/vesa.h"
#include "../drivers/font.h"
#include "../drivers/keyboard.h"
#include "../drivers/mouse.h"
#include "gui.h"

#define MAX_WIN 6
#define TITLE_H 24

typedef struct {
    char title[32];
    int x, y, w, h;
    int visible;
    int active;
} window_t;

static window_t wins[MAX_WIN];
static int win_cnt = 0;
static int active = -1;
static int running = 1;
static int drag = -1;
static int drag_x, drag_y;

static void draw_win(window_t* w) {
    if (!w->visible) return;
    
    int x = w->x, y = w->y;
    int wt = w->w, h = w->h;
    uint32_t tc = w->active ? UI_ACCENT : CLR_GRAY;
    
    /* Shadow */
    vesa_fill_rect(x + 6, y + 6, wt, h, 0x60000000);
    
    /* Body */
    vesa_fill_rect(x, y, wt, h, UI_WINDOW_BG);
    
    /* Title */
    vesa_fill_rect(x, y, wt, TITLE_H, tc);
    font_draw_string(x + 10, y + 5, w->title, CLR_WHITE);
    
    /* Close button */
    vesa_fill_rect(x + wt - 24, y + 6, 16, 12, CLR_RED);
    
    /* Border */
    vesa_rect(x, y, wt, h, tc);
}

static void draw_cursor(int x, int y) {
    uint32_t c = CLR_WHITE;
    vesa_pixel(x, y, c);
    vesa_pixel(x + 1, y, c);
    vesa_pixel(x, y + 1, c);
    vesa_pixel(x + 1, y + 1, c);
}

static void draw_desktop(void) {
    vesa_clear(UI_BG);
    
    /* Panel */
    vesa_fill_rect(0, 0, 800, 28, UI_PANEL);
    font_draw_string(10, 6, "KernelOS | 800x600 | Mouse + Keyboard", CLR_WHITE);
    
    for (int i = 0; i < win_cnt; i++) {
        draw_win(&wins[i]);
    }
}

static void create_win(const char* t, int x, int y, int w, int h) {
    if (win_cnt >= MAX_WIN) return;
    window_t* win = &wins[win_cnt];
    
    int i = 0;
    while (t[i] && i < 31) { win->title[i] = t[i]; i++; }
    win->title[i] = 0;
    
    win->x = x; win->y = y;
    win->w = w; win->h = h;
    win->visible = 1;
    win->active = (win_cnt == 0);
    
    if (win_cnt == 0) active = 0;
    win_cnt++;
}

static void close_win(int id) {
    if (id >= 0 && id < win_cnt) wins[id].visible = 0;
}

static int in_title(window_t* w, int x, int y) {
    return w->visible &&
           x >= w->x && x < w->x + w->w &&
           y >= w->y && y < w->y + TITLE_H;
}

static int in_close(window_t* w, int x, int y) {
    return x >= w->x + w->w - 24 && x < w->x + w->w - 8 &&
           y >= w->y + 6 && y < w->y + 18;
}

static void handle_mouse(void) {
    if (!mouse_moved()) return;
    
    mouse_t m = mouse_get();
    
    if (m.buttons & 1) {
        /* Check close buttons */
        for (int i = 0; i < win_cnt; i++) {
            if (wins[i].visible && in_close(&wins[i], m.x, m.y)) {
                close_win(i);
                return;
            }
        }
        
        /* Check title bars */
        for (int i = win_cnt - 1; i >= 0; i--) {
            if (wins[i].visible && in_title(&wins[i], m.x, m.y)) {
                for (int j = 0; j < win_cnt; j++) wins[j].active = 0;
                wins[i].active = 1;
                active = i;
                
                drag = i;
                drag_x = m.x - wins[i].x;
                drag_y = m.y - wins[i].y;
                return;
            }
        }
    }
    
    if (drag >= 0) {
        if (m.buttons & 1) {
            wins[drag].x = m.x - drag_x;
            wins[drag].y = m.y - drag_y;
            
            if (wins[drag].x < 0) wins[drag].x = 0;
            if (wins[drag].y < 0) wins[drag].y = 0;
            if (wins[drag].x + wins[drag].w > 800) wins[drag].x = 800 - wins[drag].w;
            if (wins[drag].y + wins[drag].h > 600) wins[drag].y = 600 - wins[drag].h;
        } else {
            drag = -1;
        }
    }
}

static void handle_kb(void) {
    char c;
    if (kb_getchar(&c)) {
        switch (c) {
            case 27: if (active >= 0) close_win(active); break;
            case '1': case '2': case '3': case '4': case '5': case '6':
                {
                    int id = c - '1';
                    if (id < win_cnt && wins[id].visible) {
                        for (int j = 0; j < win_cnt; j++) wins[j].active = 0;
                        wins[id].active = 1;
                        active = id;
                    }
                }
                break;
            case 'q': case 'Q': running = 0; break;
        }
    }
}

void gui_init(void) {
    create_win("Terminal", 50, 50, 320, 200);
    create_win("Files", 400, 50, 280, 220);
    create_win("Editor", 50, 300, 300, 200);
    create_win("Settings", 380, 300, 280, 200);
    
    kb_init();
    mouse_init();
}

void gui_run(void) {
    mouse_t m;
    
    while (running) {
        draw_desktop();
        m = mouse_get();
        draw_cursor(m.x, m.y);
        vesa_flip();
        
        handle_mouse();
        handle_kb();
        
        for (volatile int i = 0; i < 5000; i++);
    }
    
    vesa_exit();
}
