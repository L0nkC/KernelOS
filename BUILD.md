# Building KernelOS

This document describes how to build KernelOS.

## Option 1: GitHub Actions (Easiest)

The easiest way to build KernelOS is using GitHub Actions, which provides free Linux runners.

### Automatic Build on Push

Every push to `main` or `master` branch triggers an automatic build. The ISO will be available as a downloadable artifact.

### Manual Build

1. Go to the **Actions** tab in your GitHub repository
2. Select **"Build KernelOS ISO"** workflow
3. Click **"Run workflow"**
4. Download the `kernelos-iso` artifact when complete

### Creating a Release

To create a release with the ISO attached:

```bash
git tag v1.0
git push origin v1.0
```

This will automatically create a GitHub Release with the ISO and kernel binary attached.

## Option 2: Docker (Local)

If you have Docker installed on macOS:

```bash
# Install Docker Desktop
brew install --cask docker
open -a Docker

# Build using Docker
./build_with_docker.sh
```

## Option 3: macOS Native (Limited)

You can build the kernel on macOS using the cross-compiler, but the ISO creation has issues with GRUB:

```bash
# Build kernel only
make clean
make kernelos.bin

# Test kernel directly
qemu-system-i386 -kernel kernelos.bin -m 64M -vga std
```

For a full ISO, use GitHub Actions or Docker.

## Option 4: Linux VM

Build on any Linux distribution with the required packages:

```bash
sudo apt-get install -y nasm gcc-i686-linux-gnu binutils-i686-linux-gnu xorriso grub-pc-bin make
make clean
make iso
```

## Build Requirements

- NASM (assembler)
- i686-elf-gcc or i686-linux-gnu-gcc (cross-compiler)
- xorriso (ISO creation)
- grub-mkrescue (for GRUB-based ISO)
- make

## Output Files

- `kernelos.bin` - The kernel ELF binary
- `kernelos.iso` - Bootable CD image

## Testing

```bash
# Test with QEMU
qemu-system-i386 -cdrom kernelos.iso -m 64M -vga std -boot d

# Or test kernel directly
qemu-system-i386 -kernel kernelos.bin -m 64M -vga std
```
