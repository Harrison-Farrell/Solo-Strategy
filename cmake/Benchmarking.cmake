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

include(FetchContent)

FetchContent_Declare(
	googlebenchmark
	GIT_REPOSITORY https://github.com/google/benchmark.git
	GIT_TAG v1.9.5
	GIT_SHALLOW 1
)

option(BENCHMARK_ENABLE_TESTING OFF)
option(BENCHMARK_ENABLE_EXCEPTIONS ON)
option(BENCHMARK_ENABLE_LTO OFF)
option(BENCHMARK_USE_LIBCXX OFF)
option(BENCHMARK_ENABLE_WERROR OFF)
option(BENCHMARK_FORCE_WERROR OFF)
option(BENCHMARK_ENABLE_INSTALL OFF)
option(BENCHMARK_ENABLE_DOXYGEN OFF)
option(BENCHMARK_INSTALL_DOCS OFF)
option(BENCHMARK_ENABLE_GTEST_TESTS OFF)

FetchContent_MakeAvailable(googlebenchmark)

macro(AddBenchmarks target)
	add_executable(${target}
		${target}.cpp
	)

	target_link_libraries(${target}
	PRIVATE benchmark::benchmark
	PRIVATE benchmark::benchmark_main)
	set_target_properties(${target} PROPERTIES 
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/benchmark"
)
endmacro()
