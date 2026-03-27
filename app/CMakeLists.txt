# -----------------------------------------------------------------------------
# Author:      Harrison Farrell
# Project:     Solo-Strategy Trading System
# Copyright:   (c) 2026 Harrison Farrell. All Rights Reserved.
#
# Licensed under the GNU Affero General Public License v3.0 (AGPL-3.0).
# This program is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See <https://www.gnu.org/licenses/agpl-3.0.html> for full details.
# -----------------------------------------------------------------------------

set(APPLICATION main_application)

add_executable(${APPLICATION} main.cpp)

target_include_directories(${APPLICATION} PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(${APPLICATION} PUBLIC 
    solo_strategy::itch_parser
    solo_strategy::lock_free_queue
    solo_strategy::memory_map
    solo_strategy::memory_pool
    solo_strategy::utilities
    solo_strategy::thread
)

set_target_properties(${APPLICATION}
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

file(COPY ${CMAKE_SOURCE_DIR}/test/sample_set/locate-13-10k.bin DESTINATION ${CMAKE_BINARY_DIR}/bin)