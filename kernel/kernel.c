/* kernel/kernel.c
 * KernelOS - Simple Mode 13h GUI
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
extern void simple_gui_run(void);

void kernel_main(uint32_t magic, void* mbi) {
    __asm__ volatile ("mov %0, %%esp" : : "r"(kstack + sizeof(kstack)));
    
    vga_clear();
    vga_puts("KernelOS Boot\n\n");
    
    vga_puts("Init GDT...\n");
    gdt_init();
    
    vga_puts("Init IDT...\n");
    idt_init();
    
    vga_puts("Init PIC...\n");
    pic_init();
    
    vga_puts("Init Paging...\n");
    paging_init();
    
    vga_puts("\nStarting GUI...\n");
    simple_gui_run();
    
    while (1) __asm__ volatile ("cli; hlt");
}
