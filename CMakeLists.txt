# Minimum CMake version required
cmake_minimum_required(VERSION 4.1)

# Project name and version
project(SoloStrategy VERSION 1.0 LANGUAGES C CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")


# Optional: Global compiler flags for optimization
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

add_subdirectory(src)
add_subdirectory(theory_src)
add_subdirectory(test)