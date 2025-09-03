#!/bin/bash

# Run script for ImageTUI project

BUILD_DIR="build"
EXECUTABLE="image_tui"
FORCE_BUILD=false

# Check for --build flag
if [ "$1" = "--build" ] || [ "$1" = "-b" ]; then
    FORCE_BUILD=true
    shift  # Remove the flag from arguments
fi

# Function to build the project
build_project() {
    echo "Building project..."
    if [ -f "scripts/build.sh" ]; then
        ./scripts/build.sh
        return $?
    else
        echo "Build script not found. Please build manually or create scripts/build.sh"
        return 1
    fi
}

# Check if we need to build
NEED_BUILD=false

if [ "$FORCE_BUILD" = true ]; then
    echo "Force rebuild requested..."
    NEED_BUILD=true
elif [ ! -f "$BUILD_DIR/$EXECUTABLE" ]; then
    echo "Binary not found. Building first..."
    NEED_BUILD=true
else
    # Check if binary is newer than source files
    if find src/ include/ CMakeLists.txt -newer "$BUILD_DIR/$EXECUTABLE" 2>/dev/null | grep -q .; then
        echo "Source files are newer than binary. Rebuilding..."
        NEED_BUILD=true
    fi
fi

# Build if needed
if [ "$NEED_BUILD" = true ]; then
    build_project
    
    # Check if build was successful
    if [ $? -ne 0 ] || [ ! -f "$BUILD_DIR/$EXECUTABLE" ]; then
        echo "❌ Build failed. Cannot run $EXECUTABLE"
        exit 1
    fi
    echo " Build successful!"
fi

# Check if we have arguments for the executable
if [ $# -eq 0 ]; then
    echo "  No arguments provided. Usage:"
    echo "   $0 <input_image> <output_image>"
    echo "   Example: $0 test.jpg output.png"
    echo ""
    echo "Available options:"
    echo "   $0 --build          Force rebuild before running"
    echo "   $0 -b              Same as --build"
    exit 1
fi

echo " Starting ImageTUI execution..."

# Record start time
START_TIME=$(date +%s%3N)

# Run the executable
./$BUILD_DIR/$EXECUTABLE "$@"
EXIT_CODE=$?

# Record end time and calculate duration
END_TIME=$(date +%s%3N)
EXECUTION_TIME=$((END_TIME - START_TIME))

echo ""
if [ $EXIT_CODE -eq 0 ]; then
    echo " Total execution completed in ${EXECUTION_TIME}ms"
else
    echo " Execution failed with exit code: $EXIT_CODE (${EXECUTION_TIME}ms)"
fi

exit $EXIT_CODE