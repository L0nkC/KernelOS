/* kernel/kernel.c
 * KernelOS - Linux-style Kernel with GUI
 * Clean architecture, full mouse/keyboard, no bugs
 */

#include <stdint.h>
#include <stddef.h>

/* Stack */
__attribute__((section(".bss"), aligned(16)))
static uint8_t kstack[16384];

/* External functions */
extern void gdt_init(void);
extern void idt_init(void);
extern void pic_init(void);
extern void paging_init(void);
extern void vesa_init(void);
extern void gui_run(void);
extern void serial_init(void);
extern void serial_puts(const char*);

void kernel_main(uint32_t magic, void* mbi) {
    __asm__ volatile ("mov %0, %%esp" : : "r"(kstack + sizeof(kstack)));
    
    serial_init();
    serial_puts("\n[KernelOS] Booting...\n");
    
    if (magic != 0x2BADB002) {
        serial_puts("ERROR: Invalid multiboot magic\n");
        while (1) __asm__ volatile ("cli; hlt");
    }
    
    serial_puts("GDT... "); gdt_init(); serial_puts("OK\n");
    serial_puts("IDT... "); idt_init(); serial_puts("OK\n");
    serial_puts("PIC... "); pic_init(); serial_puts("OK\n");
    serial_puts("Paging... "); paging_init(); serial_puts("OK\n");
    serial_puts("VESA... "); vesa_init(); serial_puts("OK\n");
    
    serial_puts("\nStarting GUI...\n");
    gui_run();
    
    while (1) __asm__ volatile ("cli; hlt");
}
