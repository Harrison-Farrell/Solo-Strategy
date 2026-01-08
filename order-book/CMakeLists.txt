# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name (use the directory name or a descriptive name)
project(OrderBook)

# Collect all source files in the directory (adjust patterns as needed)
file(GLOB SOURCES "*.cpp" "*.cc" "*.cxx")
file(GLOB HEADERS "*.h" "*.hpp")

# Create a static library target
add_library(${PROJECT_NAME} STATIC ${SOURCES} ${HEADERS})

# Set include directories for the library (public so dependents can include headers)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR})

# Link dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC MarketOrder MemoryPool Utilities)