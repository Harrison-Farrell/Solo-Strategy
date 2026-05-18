# -----------------------------------------------------------------------------
project(book_builder_ui)

add_library(${PROJECT_NAME} STATIC)
add_library(${LIBRARY_PREFIX}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${CURRENT_CMAKE_DIR}book_builder_ui.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} INTERFACE quill::quill)

target_link_libraries(${PROJECT_NAME} PUBLIC
    solo_strategy::book_builder
    solo_strategy::utilities
    imgui_static
)
