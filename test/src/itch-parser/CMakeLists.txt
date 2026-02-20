set(MEMORY_MAP test_memory_map_file)

add_executable(${MEMORY_MAP} test_memory_map_file.cpp)

target_include_directories(${MEMORY_MAP} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${MEMORY_MAP} PUBLIC ITCH-PARSER)

set_target_properties(${MEMORY_MAP}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/test"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/test"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
)