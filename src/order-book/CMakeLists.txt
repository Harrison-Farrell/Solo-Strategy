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

# Project name
project(order_book)

add_library(${PROJECT_NAME} STATIC)
add_library(${LIBRARY_PREFIX}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_sources(${PROJECT_NAME} PRIVATE
    ${CURRENT_CMAKE_DIR}order_book.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} INTERFACE quill::quill)

target_link_libraries(${PROJECT_NAME} PUBLIC
    solo_strategy::market_order
    solo_strategy::memory_pool
    solo_strategy::utilities
)