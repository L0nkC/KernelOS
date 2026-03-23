/* kernel/isr.c
 * Exception and IRQ handlers
 */

#include <stdint.h>
#include "io.h"
#include "../drivers/vga_text.h"

typedef struct {
    uint32_t ds, edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, err_code;
    uint32_t eip, cs, eflags;
} registers_t;

static void itoa(int n, char* buf) {
    if (n == 0) {
        buf[0] = '0';
        buf[1] = 0;
        return;
    }
    int i = 0, neg = 0;
    if (n < 0) { neg = 1; n = -n; }
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    if (neg) buf[i++] = '-';
    buf[i] = 0;
    for (int j = 0; j < i/2; j++) {
        char t = buf[j];
        buf[j] = buf[i-1-j];
        buf[i-1-j] = t;
    }
}

void isr_handler(registers_t* regs) {
    vga_puts("\n*** EXCEPTION: ");
    char buf[16];
    itoa(regs->int_no, buf);
    vga_puts(buf);
    vga_puts(" ***\n");
    while (1) __asm__ volatile ("cli; hlt");
}

typedef void (*irq_handler_t)(void);
static irq_handler_t irq_handlers[16];

void irq_install_handler(int irq, irq_handler_t h) {
    if (irq >= 0 && irq < 16) irq_handlers[irq] = h;
}

void irq_handler(int irq) {
    if (irq_handlers[irq]) irq_handlers[irq]();
    if (irq >= 8) outb(0xA0, 0x20);
    outb(0x20, 0x20);
}
