#!/bin/bash
# Build KernelOS ISO using Docker (Linux environment)
# This ensures GRUB works correctly for creating bootable ISOs

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CONTAINER_NAME="kernelos_builder"
IMAGE_NAME="kernelos_build:latest"

echo "=== KernelOS Docker Build ==="
echo ""

# Check if Docker is available
if ! command -v docker &> /dev/null; then
    echo "Error: Docker is not installed or not in PATH"
    echo "Please install Docker Desktop for Mac: https://www.docker.com/products/docker-desktop"
    exit 1
fi

# Check if Docker daemon is running
if ! docker info &> /dev/null; then
    echo "Error: Docker daemon is not running"
    echo "Please start Docker Desktop"
    exit 1
fi

echo "Building Docker image..."
docker build -t "$IMAGE_NAME" -f "$SCRIPT_DIR/Dockerfile" "$SCRIPT_DIR"

echo ""
echo "Building kernel and ISO in Docker..."

# Run the build in a container
# We mount the output directory to extract the built ISO
docker run --rm \
    --name "$CONTAINER_NAME" \
    -v "$SCRIPT_DIR:/build" \
    -w /build \
    "$IMAGE_NAME" \
    bash -c "
        echo 'Cleaning...'
        make clean 2>/dev/null || true
        
        echo ''
        echo 'Building kernel...'
        make kernelos.bin
        
        echo ''
        echo 'Creating ISO...'
        make iso
        
        echo ''
        echo 'Build complete!'
        ls -la kernelos.bin kernelos.iso
    "

echo ""
echo "=== Build Complete ==="
echo ""
echo "Output files:"
ls -lh "$SCRIPT_DIR/kernelos.bin" "$SCRIPT_DIR/kernelos.iso" 2>/dev/null || true
echo ""
echo "To test the ISO:"
echo "  qemu-system-i386 -cdrom kernelos.iso -m 64M -vga std -boot d"
