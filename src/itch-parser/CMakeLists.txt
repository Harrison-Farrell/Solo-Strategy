# Project name
project(ITCH-PARSER)

add_library(${PROJECT_NAME} STATIC)

target_sources(${PROJECT_NAME} PRIVATE
    ${CURRENT_CMAKE_DIR}itch_parser.cpp
    ${CURRENT_CMAKE_DIR}memory_map_file.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} PUBLIC )