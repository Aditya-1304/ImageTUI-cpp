cmake_minimum_required(VERSION 3.15)
project(ImageTUI VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

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
include_directories(${CMAKE_SOURCE_DIR}/third_party)

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

# Main executable
add_executable(image_tui ${MAIN_SOURCES})
target_link_libraries(image_tui imagelib)

# # Example executable
# add_executable(simple_filters examples/simple_filters.cpp)
# target_link_libraries(simple_filters imagelib)

# Platform-specific linking
if(WIN32)
    # Windows specific libraries if needed
    target_link_libraries(image_tui)
else()
    # Unix-like systems
    target_link_libraries(image_tui pthread)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_libraries(image_tui dl)
    endif()
endif()

# Preprocessor definitions
target_compile_definitions(imagelib PUBLIC
    PROJECT_VERSION="${PROJECT_VERSION}"
    $<$<CONFIG:Debug>:DEBUG_BUILD>
    $<$<CONFIG:Release>:RELEASE_BUILD>
)

# Install targets
install(TARGETS image_tui DESTINATION bin)
# install(TARGETS simple_filters DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/imagetui)

# # Optional: Testing
# option(BUILD_TESTS "Build test suite" OFF)
# if(BUILD_TESTS)
#     enable_testing()
#     add_subdirectory(tests)
# endif()

# # Optional: Documentation
# find_package(Doxygen)
# if(DOXYGEN_FOUND)
#     add_custom_target(docs
#         COMMAND ${DOXYGEN_EXECUTABLE}
#         WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
#         COMMENT "Generating documentation"
#     )
# endif()

# Package configuration
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Image processing TUI application")
include(CPack)

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")