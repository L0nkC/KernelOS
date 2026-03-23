/* kernel/kernel.c */
#include <stdint.h>

extern void gui_run(void);

void kernel_main(uint32_t magic, void* mbi) {
    (void)magic;
    (void)mbi;
    
    gui_run();
    
    while (1) __asm__ volatile ("cli; hlt");
}
