/* drivers/keyboard.c
 * PS/2 Keyboard - Interrupt driven
 */

#include <stdint.h>
#include "../kernel/io.h"
#include "../kernel/isr.h"

#define PS2_DATA    0x60
#define PS2_STATUS  0x64
#define KB_BUF_SIZE 64

static volatile char kb_buf[KB_BUF_SIZE];
static volatile int kb_read = 0;
static volatile int kb_write = 0;

static const char scancode[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,  'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0,  ' ', 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static void kb_handler(void) {
    uint8_t code = inb(PS2_DATA);
    
    /* Ignore special codes and releases */
    if (code == 0xFA || code == 0xFE || code == 0xFC || (code & 0x80)) return;
    
    if (code < 128) {
        char c = scancode[code];
        if (c) {
            int next = (kb_write + 1) % KB_BUF_SIZE;
            if (next != kb_read) {
                kb_buf[kb_write] = c;
                kb_write = next;
            }
        }
    }
}

void kb_init(void) {
    while (inb(PS2_STATUS) & 0x01) inb(PS2_DATA);
    irq_install_handler(1, kb_handler);
    
    uint8_t mask = inb(0x21);
    mask &= ~(1 << 1);
    outb(0x21, mask);
}

int kb_getchar(char* c) {
    if (kb_read == kb_write) return 0;
    *c = kb_buf[kb_read];
    kb_read = (kb_read + 1) % KB_BUF_SIZE;
    return 1;
}

char kb_read_char(void) {
    char c;
    while (!kb_getchar(&c));
    return c;
}
