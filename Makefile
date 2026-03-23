# KernelOS Makefile

CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdlib -nostartfiles \
         -fno-exceptions -fno-stack-protector -I. -std=c99
AS = nasm
ASFLAGS = -f elf32
LD = ld
LDFLAGS = -m elf_i386 -T boot/linker.ld

OBJS = boot/boot.o kernel/kernel.o kernel/gdt.o kernel/gdt_flush.o \
       kernel/idt.o kernel/idt_flush.o kernel/isr.o kernel/isr_asm.o \
       kernel/paging.o kernel/paging_asm.o \
       drivers/serial.o drivers/vesa.o drivers/font.o \
       drivers/keyboard.o drivers/mouse.o gui/gui.o

TARGET = kernelos.bin
ISO = kernelos.iso

.PHONY: all clean iso run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

iso: $(TARGET)
	@mkdir -p iso/boot/grub
	@cp $(TARGET) iso/boot/kernelos.bin
	@echo 'set timeout=0' > iso/boot/grub/grub.cfg
	@echo 'set default=0' >> iso/boot/grub/grub.cfg
	@echo 'menuentry "KernelOS" {' >> iso/boot/grub/grub.cfg
	@echo '    multiboot /boot/kernelos.bin' >> iso/boot/grub/grub.cfg
	@echo '    boot' >> iso/boot/grub/grub.cfg
	@echo '}' >> iso/boot/grub/grub.cfg
	@grub-mkrescue -o $(ISO) iso 2>&1 | grep -v "^xorriso" || true

run: iso
	qemu-system-i386 -cdrom $(ISO) -m 64M -vga std

debug: iso
	qemu-system-i386 -cdrom $(ISO) -m 64M -vga std -serial stdio

clean:
	@rm -f $(OBJS) $(TARGET) $(ISO)
	@rm -rf iso

boot/boot.o: boot/boot.asm
kernel/gdt_flush.o: kernel/gdt_flush.asm
kernel/idt_flush.o: kernel/idt_flush.asm
kernel/isr_asm.o: kernel/isr_asm.asm
kernel/paging_asm.o: kernel/paging.asm
