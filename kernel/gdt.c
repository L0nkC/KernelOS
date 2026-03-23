/* kernel/gdt.c */
#include <stdint.h>

struct gdt_entry { uint16_t l, b; uint8_t m, a, g, h; };
struct gdt_ptr { uint16_t l; uint32_t b; } __attribute__((packed));

static struct gdt_entry gdt[3];
static struct gdt_ptr gp;
extern void gdt_flush(uint32_t);

static void set(int n, uint32_t b, uint32_t l, uint8_t a, uint8_t g) {
    gdt[n].l = l & 0xFFFF;
    gdt[n].b = b & 0xFFFF;
    gdt[n].m = (b >> 16) & 0xFF;
    gdt[n].h = (b >> 24) & 0xFF;
    gdt[n].g = ((l >> 16) & 0x0F) | (g & 0xF0);
    gdt[n].a = a;
}

void gdt_init(void) {
    set(0, 0, 0, 0, 0);
    set(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    set(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gp.l = sizeof(gdt) - 1;
    gp.b = (uint32_t)gdt;
    gdt_flush((uint32_t)&gp);
}
