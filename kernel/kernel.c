/* kernel/kernel.c
 * KernelOS - Linux-style Kernel with GUI
 */

#include <stdint.h>
#include <stddef.h>
#include "../drivers/vga_text.h"

/* Stack */
__attribute__((section(".bss"), aligned(16)))
static uint8_t kstack[16384];

extern void gdt_init(void);
extern void idt_init(void);
extern void pic_init(void);
extern void paging_init(void);
extern int vesa_init(void);
extern void gui_run(void);

void kernel_main(uint32_t magic, void* mbi) {
    __asm__ volatile ("mov %0, %%esp" : : "r"(kstack + sizeof(kstack)));
    
    vga_clear();
    vga_puts("KernelOS Boot\n");
    
    vga_puts("Magic: ");
    vga_puthex(magic);
    vga_puts("\n");
    
    if (magic != 0x2BADB002) {
        vga_puts("BAD MAGIC!\n");
        while (1) __asm__ volatile ("cli; hlt");
    }
    
    vga_puts("GDT...");
    gdt_init();
    vga_puts("OK\n");
    
    vga_puts("IDT...");
    idt_init();
    vga_puts("OK\n");
    
    vga_puts("PIC...");
    pic_init();
    vga_puts("OK\n");
    
    vga_puts("Paging...");
    paging_init();
    vga_puts("OK\n");
    
    vga_puts("VESA...");
    int vesa = vesa_init();
    if (vesa == 0) {
        vga_puts("OK\n");
    } else {
        vga_puts("FAIL\n");
    }
    
    vga_puts("Starting GUI...\n");
    gui_run();
    
    vga_puts("GUI exited\n");
    while (1) __asm__ volatile ("cli; hlt");
}
