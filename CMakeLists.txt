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

# Minimum CMake version required
cmake_minimum_required(VERSION 3.31)

# Project name and version
project(SoloStrategy VERSION 1.0 LANGUAGES C CXX)

# Set C++ standard globally
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set global library ALIAS
set(LIBRARY_PREFIX "solo_strategy")

# Build options
option(BUILD_DOXYGEN "Build Doxygen documentation" ON)
option(BUILD_TESTS "Build unit tests" ON)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
option(BUILD_EXAMPLES "Build examples" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Check the Compiler Type
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-O0 -g)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(-O3 -march=native)
    endif()

elseif(MSVC)
    add_compile_options(/W4)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(/Od /Zi)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_compile_options(/Ox)
    endif()
endif()


add_subdirectory(src)
add_subdirectory(app)

if(BUILD_TESTS)
    add_subdirectory(test)
endif()

if(BUILD_BENCHMARKS)
    add_subdirectory(theory_src)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()