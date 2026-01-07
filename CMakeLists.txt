# Top-level CMakeLists.txt for the Solo-Strategy project
# This file orchestrates building static libraries from subdirectories.

# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(SoloStrategy VERSION 1.0 LANGUAGES CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Optional: Global compiler flags for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

# Add subdirectories (each should contain its own CMakeLists.txt for a static library)
add_subdirectory(utilities)
add_subdirectory(memory-pool)
add_subdirectory(order-book)

# Optional: Create an overall target that depends on all libraries (for convenience)
# This allows building all libs with a single command like 'make all_libs'
add_custom_target(all_libs DEPENDS Utilities MemoryPool OrderBook)