/* kernel/kernel.c */
#include <stdint.h>

extern void gui_run(void);

void kernel_main(uint32_t magic, void* mbi) {
    (void)magic;
    (void)mbi;
    
    gui_run();
    
    __asm__ volatile ("outb %%al, $0xf4" : : "a"((uint8_t)0x01));
    while (1) __asm__ volatile ("cli; hlt");
}
