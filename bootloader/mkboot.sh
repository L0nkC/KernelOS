#!/bin/bash
# Build custom bootable ISO without GRUB

set -e

WORK_DIR="/tmp/custom_iso"
OUTPUT_ISO="/Users/kalihome/Programming/KernelOS/kernelos_custom.iso"
KERNEL_ELF="/Users/kalihome/Programming/KernelOS/kernelos.bin"
BOOTLOADER_DIR="/Users/kalihome/Programming/KernelOS/bootloader"

echo "=== Building Custom Bootloader ISO ==="

# Create working directory
rm -rf "$WORK_DIR"
mkdir -p "$WORK_DIR"

# Convert ELF kernel to flat binary
echo "Converting kernel to flat binary..."
i686-elf-objcopy -O binary "$KERNEL_ELF" "$WORK_DIR/kernel.bin"

# Pad kernel to multiple of 2048 bytes (CD sector)
KERNEL_SIZE=$(stat -f%z "$WORK_DIR/kernel.bin" 2>/dev/null || stat -c%s "$WORK_DIR/kernel.bin")
PADDING=$((2048 - (KERNEL_SIZE % 2048)))
if [ $PADDING -ne 2048 ]; then
    dd if=/dev/zero bs=1 count=$PADDING >> "$WORK_DIR/kernel.bin" 2>/dev/null
fi

echo "Kernel size: $KERNEL_SIZE bytes (padded to $(( (KERNEL_SIZE + PADDING) / 2048 )) sectors)"

# Assemble bootloader
echo "Assembling bootloader..."
nasm -f bin "$BOOTLOADER_DIR/boot.asm" -o "$WORK_DIR/boot.bin"

# Create the ISO image
# Layout:
# Sector 0: Boot image (2048 bytes)
# Sector 1+: Kernel
echo "Creating ISO..."

# Combine bootloader and kernel
cat "$WORK_DIR/boot.bin" "$WORK_DIR/kernel.bin" > "$WORK_DIR/boot_image.bin"

# Create ISO with El Torito using the combined image
xorriso -as mkisofs \
    -iso-level 3 \
    -full-iso9660-filenames \
    -volid "KernelOS" \
    -eltorito-boot boot.bin \
    -eltorito-catalog boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -o "$OUTPUT_ISO" \
    -graft-points \
    /boot.bin="$WORK_DIR/boot.bin" \
    /kernel.bin="$WORK_DIR/kernel.bin" \
    2>&1 | tail -5

echo ""
echo "=== ISO Created ==="
echo "Output: $OUTPUT_ISO"
ls -lh "$OUTPUT_ISO"
