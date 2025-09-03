#!/bin/bash

# Build script for ImageTUI project

BUILD_DIR="build"
BUILD_TYPE=${1:-Release}
CLEAN=${2:-false}

echo "Building ImageTUI project in $BUILD_TYPE mode..."

# Clean build directory if requested
if [ "$CLEAN" = "clean" ] || [ "$2" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf $BUILD_DIR
fi

# Create build directory
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Configure with CMake
echo "Configuring project with CMake..."
cmake -DCMAKE_BUILD_TYPE=$BUILD_TYPE ..

if [ $? -ne 0 ]; then
    echo " CMake configuration failed!"
    exit 1
fi

# Detect number of cores for parallel build
if command -v nproc >/dev/null 2>&1; then
    CORES=$(nproc)
elif command -v sysctl >/dev/null 2>&1; then
    CORES=$(sysctl -n hw.ncpu)
else
    CORES=4
fi

echo "Building with $CORES cores..."

# Build the project
make -j$CORES

if [ $? -eq 0 ]; then
    echo " Build successful!"
    echo "Executable: $BUILD_DIR/image_tui"

    
    # Show executable info
    if [ -f "image_tui" ]; then
        echo "Executable size: $(du -h image_tui | cut -f1)"
        echo "Run with: ./$BUILD_DIR/image_tui"
    fi
else
    echo " Build failed!"
    exit 1
fi