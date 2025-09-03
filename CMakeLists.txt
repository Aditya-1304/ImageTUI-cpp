cmake_minimum_required(VERSION 3.15)
project(ImageTUI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)  

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenCV 4 REQUIRED COMPONENTS core imgcodecs imgproc)
message(STATUS "OpenCV: ${OpenCV_VERSION}")
message(STATUS "OpenCV include dirs: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs (selected): ${OpenCV_LIBS}")


# Compiler-specific flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -pedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-g -O0 -fsanitize=address)
        add_link_options(-fsanitize=address)
    else()
        add_compile_options(-O3 -DNDEBUG)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi)
    else()
        add_compile_options(/O2 /DNDEBUG)
    endif()
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${OpenCV_INCLUDE_DIRS})


# Core library sources
set(CORE_SOURCES
    src/core/image_processor.cpp
)

# Filter sources
set(FILTER_SOURCES
    src/filters/artistic.cpp
    src/filters/basic.cpp
    src/filters/color.cpp
    src/filters/enhancement.cpp
    src/filters/geometric.cpp
)

# UI sources
set(UI_SOURCES
    src/ui/tui.cpp
)

# Utility sources
set(UTIL_SOURCES
    src/utils/utility.cpp
)

# Main executable sources
set(MAIN_SOURCES
    src/main.cpp
)

# Combine all library sources
set(LIB_SOURCES 
    ${CORE_SOURCES}
    ${FILTER_SOURCES}
    ${UI_SOURCES}
    ${UTIL_SOURCES}
)

# Create static library for better organization
add_library(imagelib STATIC ${LIB_SOURCES})

target_include_directories(imagelib PUBLIC
  ${CMAKE_SOURCE_DIR}/include
  ${OpenCV_INCLUDE_DIRS}
)
target_link_libraries(imagelib PUBLIC ${OpenCV_LIBS})

# Main executable
add_executable(image_tui ${MAIN_SOURCES})

target_include_directories(image_tui PRIVATE ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(image_tui PRIVATE imagelib)


if(UNIX AND NOT APPLE)
  target_link_libraries(image_tui PRIVATE pthread)
endif()


# Preprocessor definitions
target_compile_definitions(imagelib PUBLIC
    PROJECT_VERSION="${PROJECT_VERSION}"
    OPENCV_ENABLED=1
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)

# Install targets
install(TARGETS image_tui DESTINATION bin)
# install(TARGETS simple_filters DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/imagetui)

# Package configuration
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Image processing TUI application")
include(CPack)

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

