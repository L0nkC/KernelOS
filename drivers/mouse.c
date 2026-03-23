/* drivers/mouse.c
 * PS/2 Mouse - 3-button with interrupts
 */

#include <stdint.h>
#include "../kernel/io.h"
#include "../kernel/isr.h"
#include "mouse.h"

#define PS2_CMD     0x64
#define PS2_DATA    0x60

static volatile mouse_t mouse = {400, 300, 0, 0};
static uint8_t mouse_cycle = 0;
static int8_t mouse_packet[4];

static void mouse_wait(uint8_t type) {
    int t = 100000;
    if (type == 0) {
        while (t-- && (inb(PS2_CMD) & 0x02));
    } else {
        while (t-- && !(inb(PS2_CMD) & 0x01));
    }
}

static void mouse_write(uint8_t d) {
    mouse_wait(0);
    outb(PS2_CMD, 0xD4);
    mouse_wait(0);
    outb(PS2_DATA, d);
}

static uint8_t mouse_read(void) {
    mouse_wait(1);
    return inb(PS2_DATA);
}

static void mouse_handler(void) {
    uint8_t data = inb(PS2_DATA);
    
    switch (mouse_cycle) {
        case 0:
            if ((data & 0x08) == 0) return;
            mouse_packet[0] = data;
            mouse_cycle = 1;
            break;
        case 1:
            mouse_packet[1] = data;
            mouse_cycle = 2;
            break;
        case 2:
            mouse_packet[2] = data;
            mouse_cycle = 0;
            
            mouse.buttons = mouse_packet[0] & 0x07;
            
            int16_t dx = mouse_packet[1];
            if (mouse_packet[0] & 0x10) dx -= 256;
            
            int16_t dy = mouse_packet[2];
            if (mouse_packet[0] & 0x20) dy -= 256;
            dy = -dy;
            
            mouse.x += dx;
            mouse.y += dy;
            
            if (mouse.x < 0) mouse.x = 0;
            if (mouse.y < 0) mouse.y = 0;
            if (mouse.x >= 800) mouse.x = 799;
            if (mouse.y >= 600) mouse.y = 599;
            
            mouse.changed = 1;
            break;
    }
}

void mouse_init(void) {
    mouse_wait(0);
    outb(PS2_CMD, 0xA8);
    
    mouse_wait(0);
    outb(PS2_CMD, 0x20);
    mouse_wait(1);
    uint8_t s = inb(PS2_DATA) | 2;
    mouse_wait(0);
    outb(PS2_CMD, 0x60);
    mouse_wait(0);
    outb(PS2_DATA, s);
    
    mouse_write(0xF6);
    mouse_read();
    mouse_write(0xF4);
    mouse_read();
    
    irq_install_handler(12, mouse_handler);
    
    uint8_t mask = inb(0xA1);
    mask &= ~(1 << 4);
    outb(0xA1, mask);
}

mouse_t mouse_get(void) {
    mouse.changed = 0;
    return mouse;
}

int mouse_moved(void) {
    return mouse.changed;
}
