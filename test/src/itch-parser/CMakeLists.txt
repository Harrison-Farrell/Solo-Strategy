set(MEMORY_MAP test_memory_map_file)

add_executable(${MEMORY_MAP} test_memory_map_file.cpp)

target_include_directories(${MEMORY_MAP} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${MEMORY_MAP} PUBLIC  ITCH-PARSER)