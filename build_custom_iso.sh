#!/bin/bash
set -e

echo "=== Building Custom KernelOS ISO ==="

WORKDIR="/tmp/kernelos_custom_build"
rm -rf "$WORKDIR"
mkdir -p "$WORKDIR"

# 1. Build kernel with custom linker script (load at 0x10000)
echo "Building kernel for 0x10000..."
make clean
make kernelos.bin LDFLAGS="-m elf_i386 -T boot/linker_boot.ld" 2>&1 | tail -5

# 2. Convert to flat binary
echo "Converting to flat binary..."
i686-elf-objcopy -O binary kernelos.bin "$WORKDIR/kernel.flat"

# Pad to 2048 boundary
SIZE=$(stat -f%z "$WORKDIR/kernel.flat" 2>/dev/null || stat -c%s "$WORKDIR/kernel.flat")
PADDING=$((2048 - SIZE % 2048))
if [ $PADDING -ne 2048 ]; then
    dd if=/dev/zero bs=1 count=$PADDING >> "$WORKDIR/kernel.flat" 2>/dev/null
fi

# 3. Assemble bootloader
echo "Assembling bootloader..."
nasm -f bin bootloader/final_boot.asm -o "$WORKDIR/boot.bin"

# 4. Create combined image
echo "Creating boot image..."
cat "$WORKDIR/boot.bin" "$WORKDIR/kernel.flat" > "$WORKDIR/boot.img"

# 5. Create ISO
echo "Creating ISO..."
xorriso -as mkisofs \
    -iso-level 3 \
    -full-iso9660-filenames \
    -volid "KernelOS" \
    -b boot.img \
    -c boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -o kernelos_custom.iso \
    -graft-points \
    boot.img="$WORKDIR/boot.img" \
    2>&1 | tail -3

echo ""
echo "=== Done ==="
ls -lh kernelos_custom.iso
echo ""
echo "Test: qemu-system-i386 -cdrom kernelos_custom.iso -m 64M -vga std -boot d"
