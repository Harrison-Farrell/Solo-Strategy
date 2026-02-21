enable_testing()

include(FetchContent)
FetchContent_Declare(
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG v1.17.0
	GIT_SHALLOW 1
)

# Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

option(INSTALL_GMOCK OFF)
option(INSTALL_GTEST OFF)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

macro(AddTests target)
	target_link_libraries(${target} PRIVATE gtest_main gmock)
	gtest_discover_tests(${target})
	set_target_properties(${target} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/test"
	)
endmacro()
