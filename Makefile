# KernelOS Makefile

AS = nasm
ASFLAGS = -f elf32
LD = ld
LDFLAGS = -m elf_i386 -T boot/linker.ld

TARGET = kernelos.bin
ISO = kernelos.iso

.PHONY: all clean iso run

all: $(TARGET)

$(TARGET): boot/boot.asm boot/linker.ld
	$(AS) $(ASFLAGS) boot/boot.asm -o boot/boot.o
	$(LD) $(LDFLAGS) -o $(TARGET) boot/boot.o

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
	@grub-mkrescue -o $(ISO) iso 2>&1 | grep -v "^xorriso" || true
	@echo "ISO created: $(ISO)"

run: iso
	qemu-system-i386 -cdrom $(ISO) -m 64M -vga std -boot d

clean:
	@rm -f $(TARGET) $(ISO) boot/boot.o
	@rm -rf iso
