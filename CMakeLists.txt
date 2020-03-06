include(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.conan.txt)

cmake_minimum_required (VERSION 3.10)
project(fea_unsigned_map VERSION 1.0.0 LANGUAGES CXX)

include(ExternalProject)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(GoogleTest)

# Require up-to-date compiler.
set(CMAKE_CXX_STANDARD 14)

# Output binary to predictable location (fixes cyclic dependency issues).
set(BINARY_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${BINARY_OUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Organize unrelated targets to clean IDE hierarchy.
set(DEPENDENCY_FOLDER "Dependencies")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ${DEPENDENCY_FOLDER})

# Project
file(GLOB_RECURSE HEADER_FILES "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/*.hpp")
add_library(${PROJECT_NAME} INTERFACE)
target_sources(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${HEADER_FILES}>
)
target_include_directories(${PROJECT_NAME} INTERFACE
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
	target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror -Wno-gnu-zero-variadic-macro-arguments)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	target_compile_options(${PROJECT_NAME} INTERFACE -Wall -Wextra -Wpedantic -Werror)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	target_compile_options(${PROJECT_NAME} INTERFACE /W4 /WX /EHsc /Zc:__cplusplus /FAs /permissive-)
	target_compile_definitions(${PROJECT_NAME} INTERFACE NOMINMAX)
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.clang-format ${CMAKE_CURRENT_BINARY_DIR}/.clang-format COPYONLY)

# Install Package Configuration
install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}_targets)

install(EXPORT ${PROJECT_NAME}_targets
	NAMESPACE ${PROJECT_NAME}::
	FILE ${PROJECT_NAME}-config.cmake
	DESTINATION "${CMAKE_INSTALL_DATADIR}/cmake/${PROJECT_NAME}"
)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")


##
# External projects
##

function(output_file REQUIRED_ARG)
	file(READ ${REQUIRED_ARG} FILE)
	message("\n${REQUIRED_ARG} :\n${FILE}\n")
endfunction()

# Tests
option(BUILD_TESTING "Build and run tests." On)
if (${BUILD_TESTING})
	enable_testing()
	
	include(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
	conan_basic_setup(TARGETS)

	output_file(${CMAKE_BINARY_DIR}/conanbuildinfo_multi.cmake)
	output_file(${CMAKE_BINARY_DIR}/conanbuildinfo_release.cmake)
	output_file(${CMAKE_BINARY_DIR}/conanbuildinfo_debug.cmake)

	set(TEST_NAME ${PROJECT_NAME}_tests)
	file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp" "tests/*.c" "tests/*.hpp" "tests/*.h" "tests/*.tpp")
	add_executable(${TEST_NAME} ${TEST_SOURCES})
	message("conan include dirs : ${CONAN_INCLUDE_DIRS}")
	target_link_libraries(${TEST_NAME} PRIVATE ${PROJECT_NAME} CONAN_PKG::GTest)
	gtest_discover_tests(${TEST_NAME})
	add_dependencies(${TEST_NAME} ${PROJECT_NAME})

	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		# using Intel C++
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(${TEST_NAME} PRIVATE /W4 /WX /EHsc /Zc:__cplusplus /FAs /permissive-)
		target_compile_definitions(${TEST_NAME} PRIVATE NOMINMAX)
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEST_NAME})
	endif()
endif()

# Benchmarks
option(BUILD_BENCHMARKS "Build and run benchmarks." Off)
if (${BUILD_BENCHMARKS})

	set(LIB_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/include)
	ExternalProject_Add(bench_util_ext
		GIT_REPOSITORY https://github.com/p-groarke/bench_util.git
		CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release #${CMAKE_ARGS}
			-DCMAKE_INSTALL_PREFIX=${CMAKE_CURRENT_BINARY_DIR}
			-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}
			-DBUILD_TESTING=Off
		UPDATE_COMMAND ""
	)
	set_target_properties(bench_util_ext PROPERTIES FOLDER ${DEPENDENCY_FOLDER})

	set(BENCHMARK_NAME ${PROJECT_NAME}_benchmarks)
	file(GLOB_RECURSE BENCHMARK_SOURCES "benchmarks/*.cpp" "benchmarks/*.c" "benchmarks/*.hpp" "benchmarks/*.h" "benchmarks/*.tpp")
	add_executable(${BENCHMARK_NAME} ${BENCHMARK_SOURCES})
	target_link_libraries(${BENCHMARK_NAME} PRIVATE ${PROJECT_NAME})
	target_include_directories(${BENCHMARK_NAME} PUBLIC ${LIB_INCLUDE_DIR})
	# add_dependencies(${BENCHMARK_NAME} bench_util_ext)
	add_dependencies(${BENCHMARK_NAME} ${PROJECT_NAME} bench_util_ext)

	if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
		target_compile_options(${BENCHMARK_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
		target_compile_options(${BENCHMARK_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
		# using Intel C++
	elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		target_compile_options(${BENCHMARK_NAME} PRIVATE /W4 /WX /EHsc /Zc:__cplusplus /FAs /permissive-)
		target_compile_definitions(${BENCHMARK_NAME} PRIVATE NOMINMAX)
	endif()
endif()
