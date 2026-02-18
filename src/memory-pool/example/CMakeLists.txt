# Project name
project(MemoryPoolExample)

add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
${CURRENT_CMAKE_DIR}example_memory_pool.cpp
)

target_link_libraries(${PROJECT_NAME} PRIVATE MemoryPool)

set_target_properties(${PROJECT_NAME}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/example"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/example"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/example"
)