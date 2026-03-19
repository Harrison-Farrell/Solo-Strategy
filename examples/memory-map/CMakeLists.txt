# ------------------------------------------------------------------------------
# Author:      Harrison Farrell
# Project:     Solo-Strategy Trading System
# Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
#
# Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
# This program is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
# ------------------------------------------------------------------------------

set(MEMORY_MAP example_memory_map_file)

add_executable(${MEMORY_MAP} example_memory_map_file.cpp)

target_include_directories(${MEMORY_MAP} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${MEMORY_MAP} PUBLIC MemoryMap)

set_target_properties(${MEMORY_MAP}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/example"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/example"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/example"
)