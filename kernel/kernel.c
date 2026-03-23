/* kernel/kernel.c */
#include <stdint.h>

extern void gdt_init(void);
extern void idt_init(void);
extern void pic_init(void);
extern void paging_init(void);
extern void gui_run(void);

static volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
static int vga_pos = 0;

static void print(const char* s) {
    while (*s) {
        vga[vga_pos++] = 0x1F00 | *s++;
    }
}

void kernel_main(uint32_t magic, void* mbi) {
    (void)magic;
    (void)mbi;
    
    /* Clear screen */
    for (int i = 0; i < 2000; i++) vga[i] = 0x1F00 | ' ';
    
    print("[1]");
    gdt_init();
    print("[2]");
    idt_init();
    print("[3]");
    pic_init();
    print("[4]");
    paging_init();
    print("[5]");
    gui_run();
    print("[DONE]");
    
    __asm__ volatile ("outb %0, $0xf4" : : "a"((uint8_t)0x01));
    while (1) __asm__ volatile ("cli; hlt");
}
