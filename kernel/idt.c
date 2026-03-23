/* kernel/idt.c */
#include <stdint.h>
#include "io.h"

struct idt_entry { uint16_t bl, s; uint8_t z, f, bh; } __attribute__((packed));
struct idt_ptr { uint16_t l; uint32_t b; } __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr ip;

extern void idt_flush(uint32_t);
extern void isr0(void);

static void set(uint8_t n, uint32_t b, uint16_t s, uint8_t f) {
    idt[n].bl = b & 0xFFFF;
    idt[n].bh = (b >> 16) & 0xFFFF;
    idt[n].s = s;
    idt[n].z = 0;
    idt[n].f = f;
}

void idt_init(void) {
    for (int i = 0; i < 256; i++) set(i, 0, 0, 0);
    set(0, (uint32_t)isr0, 0x08, 0x8E);
    ip.l = sizeof(idt) - 1;
    ip.b = (uint32_t)idt;
    idt_flush((uint32_t)&ip);
}

void pic_init(void) {
    outb(0x20, 0x11); outb(0xA0, 0x11);
    outb(0x21, 0x20); outb(0xA1, 0x28);
    outb(0x21, 0x04); outb(0xA1, 0x02);
    outb(0x21, 0x01); outb(0xA1, 0x01);
    outb(0x21, 0xFF); outb(0xA1, 0xFF);
}
