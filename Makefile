# KernelOS Makefile
# Supports both macOS (with cross-compiler) and Linux

# Detect OS
UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
    # macOS with Homebrew cross-compiler
    CC = i686-elf-gcc
    LD = i686-elf-ld
    GRUB_MKRESCUE = /opt/homebrew/Cellar/i686-elf-grub/2.12/bin/i686-elf-grub-mkrescue
else
    # Linux (GitHub Actions, etc.)
    CC = gcc
    LD = ld
    GRUB_MKRESCUE = grub-mkrescue
endif

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -nostdlib -nostartfiles \
         -fno-exceptions -fno-stack-protector -fno-pic -Ikernel -std=c99
AS = nasm
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T boot/linker.ld

TARGET = kernelos.bin
ISO = kernelos.iso

OBJS = boot/boot.o \
       kernel/kernel.o kernel/gdt.o kernel/gdt_flush.o \
       kernel/idt.o kernel/idt_flush.o kernel/isr.o kernel/isr_asm.o \
       kernel/paging.o kernel/paging_asm.o \
       gui/gui.o

.PHONY: all clean iso run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

kernel/paging_asm.o: kernel/paging.asm
	$(AS) $(ASFLAGS) $< -o $@

iso: $(TARGET)
	@rm -rf iso
	@mkdir -p iso/boot/grub
	@cp $(TARGET) iso/boot/kernelos.bin
	@echo 'set timeout=0' > iso/boot/grub/grub.cfg
	@echo 'set default=0' >> iso/boot/grub/grub.cfg
	@echo '' >> iso/boot/grub/grub.cfg
	@echo 'menuentry "KernelOS" {' >> iso/boot/grub/grub.cfg
	@echo '    multiboot /boot/kernelos.bin' >> iso/boot/grub/grub.cfg
	@echo '    boot' >> iso/boot/grub/grub.cfg
	@echo '}' >> iso/boot/grub/grub.cfg
	@$(GRUB_MKRESCUE) -o $(ISO) iso 2>&1 | grep -v "^xorriso" || true
	@echo "ISO created: $(ISO)"

run: iso
	qemu-system-i386 -cdrom $(ISO) -m 64M -vga std -boot d

clean:
	@rm -f $(TARGET) $(ISO) $(OBJS)
	@rm -rf iso
