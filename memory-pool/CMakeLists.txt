# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name (use the directory name or a descriptive name)
project(MemoryPool)

# Collect all source files in the directory (adjust patterns as needed)
file(GLOB HEADERS "*.h" "*.hpp")

# Create a static library target
add_library(${PROJECT_NAME} INTERFACE ${HEADERS})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_SOURCE_DIR})

add_subdirectory(example)