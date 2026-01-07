# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name for the example
project(MemoryPoolExample)

# Collect source files (adjust if your files have different names)
file(GLOB SOURCES "*.cpp")

# Create an executable target
add_executable(${PROJECT_NAME} ${SOURCES})

# Link to any dependencies
target_link_libraries(${PROJECT_NAME} PUBLIC MemoryPool)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/example"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/example"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/example"
)