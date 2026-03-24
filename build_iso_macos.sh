#!/bin/bash
# Build ISO on macOS using available tools

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKDIR="/tmp/kernelos_iso_build"

echo "=== KernelOS macOS ISO Builder ==="
echo ""

# Clean up
rm -rf "$WORKDIR"
mkdir -p "$WORKDIR"

# Step 1: Build the kernel
echo "Step 1: Building kernel..."
cd "$SCRIPT_DIR"
make clean 2>/dev/null || true
make kernelos.bin

# Step 2: Convert kernel to flat binary
echo "Step 2: Preparing kernel binary..."
i686-elf-objcopy -O binary kernelos.bin "$WORKDIR/kernel.bin"

# Pad to sector boundary
SIZE=$(stat -f%z "$WORKDIR/kernel.bin" 2>/dev/null || stat -c%s "$WORKDIR/kernel.bin")
PADDING=$((2048 - SIZE % 2048))
if [ $PADDING -ne 2048 ]; then
    dd if=/dev/zero bs=1 count=$PADDING >> "$WORKDIR/kernel.bin" 2>/dev/null
fi
echo "  Kernel: $SIZE bytes ($(($(stat -f%z "$WORKDIR/kernel.bin" 2>/dev/null || stat -c%s "$WORKDIR/kernel.bin") / 2048)) sectors"

# Step 3: Create bootloader
echo "Step 3: Creating bootloader..."
nasm -f bin "$SCRIPT_DIR/bootloader/eltorito_boot.asm" -o "$WORKDIR/boot.bin"

# Step 4: Create combined boot image
echo "Step 4: Creating boot image..."
cat "$WORKDIR/boot.bin" "$WORKDIR/kernel.bin" > "$WORKDIR/boot.img"

# Step 5: Create ISO
echo "Step 5: Creating ISO..."
xorriso -as mkisofs \
    -iso-level 3 \
    -full-iso9660-filenames \
    -volid "KernelOS" \
    -b boot.img \
    -c boot.cat \
    -no-emul-boot \
    -boot-load-size 4 \
    -boot-info-table \
    -o "$SCRIPT_DIR/kernelos_macos.iso" \
    -graft-points \
    boot.img="$WORKDIR/boot.img" \
    2>&1 | grep -E "(Written|ISO image)"

echo ""
echo "=== Build Complete ==="
echo ""
ls -lh "$SCRIPT_DIR/kernelos_macos.iso"
echo ""
echo "Test with:"
echo "  qemu-system-i386 -cdrom kernelos_macos.iso -m 64M -vga std -boot d"
