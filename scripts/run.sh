#!/bin/bash

# Run script for ImageTUI project

BUILD_DIR="build"
EXECUTABLE="image_tui"

# Check if binary exists
if [ ! -f "$BUILD_DIR/$EXECUTABLE" ]; then
    echo "Binary not found. Building first..."
    
    # Check if build script exists
    if [ -f "scripts/build.sh" ]; then
        ./scripts/build.sh
    else
        echo "Build script not found. Please build manually or create scripts/build.sh"
        exit 1
    fi
    
    # Check if build was successful
    if [ ! -f "$BUILD_DIR/$EXECUTABLE" ]; then
        echo "Build failed. Cannot run $EXECUTABLE"
        exit 1
    fi
fi

echo "Running $EXECUTABLE..."
./$BUILD_DIR/$EXECUTABLE "$@"