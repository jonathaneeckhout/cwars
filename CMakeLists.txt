cmake_minimum_required(VERSION 3.10)

project(CWars)

# Add the include directory to the include path
include_directories(include)

# Set the source directories
set(SERVER_SOURCE_DIR src/server)
set(CLIENT_SOURCE_DIR src/client)
set(COMMON_SOURCE_DIR src/common)

# Add all source files in the server directory to the SERVER_SOURCE_FILES variable
file(GLOB SERVER_SOURCE_FILES "${SERVER_SOURCE_DIR}/*.c")

# Add all source files in the client directory to the CLIENT_SOURCE_FILES variable
file(GLOB CLIENT_SOURCE_FILES "${CLIENT_SOURCE_DIR}/*.c")

# Add all source files in the common directory to the COMMON_SOURCE_FILES variable
file(GLOB COMMON_SOURCE_FILES "${COMMON_SOURCE_DIR}/*.c")

set(CMAKE_BUILD_TYPE Debug)

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Set the compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -pedantic -std=c99")

# Set the compiler flags for the debug build
set(CMAKE_C_FLAGS_DEBUG "-g -O0 -fprofile-arcs -ftest-coverage")

# Set the compiler flags for the release build
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_BUILD_TYPE Debug)

# Create a library from the common source files
add_library(cwars_common STATIC ${COMMON_SOURCE_FILES})

target_link_libraries(cwars_common
    m
)

# Create an executable for cwars_server
add_executable(cwars_server ${SERVER_SOURCE_FILES})

target_link_libraries(cwars_server
    cwars_common
)

add_library(cwars_client SHARED ${CLIENT_SOURCE_FILES})

target_link_libraries(cwars_client
    cwars_common
)

# Add CMocka library
find_package(CMocka REQUIRED)

enable_testing()

# Function to add a test
function(add_unit_test target target_sources)
    add_executable(${target} ${target_sources} ${ARGN})
    target_link_libraries(${target} ${CMOCKA_LIBRARIES} m)
    add_test(NAME ${target} COMMAND ${target})
endfunction()

add_unit_test(test_linked_list test/linked_list/test_linked_list.c src/common/linked_list.c)
add_unit_test(test_utils test/utils/test_utils.c src/common/utils.c)
add_unit_test(test_vector test/vector/test_vector.c src/common/vector.c)

# Create COVERAGE variable
set(COVERAGE_DIR ${CMAKE_BINARY_DIR}/coverage)

add_custom_target(gcov
    COMMAND mkdir -p ${COVERAGE_DIR}
    COMMAND mkdir -p ${COVERAGE_DIR}/report
    COMMAND ${CMAKE_MAKE_PROGRAM}
    COMMAND ${CMAKE_MAKE_PROGRAM} test
)

add_custom_command(TARGET gcov
    COMMAND echo "=================== GCOV ===================="
    COMMAND cd ${COVERAGE_DIR} && find ../CMakeFiles -type f -iname "*.o" -exec gcov -c -b -f --long-file-names --preserve-paths {} "\;"
    COMMAND cd ${COVERAGE_DIR} && gcovr -k -p -r ${CMAKE_SOURCE_DIR} -s -g --html --html-details --exclude '.*test.*' -o ${COVERAGE_DIR}/report/index.html
    COMMAND cd ${COVERAGE_DIR} && gcovr -k -p -r ${CMAKE_SOURCE_DIR} -s -g --exclude '.*test.*' | tee ${COVERAGE_DIR}/report/gcovr_summary.txt
)

add_custom_target(scrub
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND rm -f ${OBJECT_DIR}/*.gcno
    COMMAND rm -f ${OBJECT_DIR}/*.gcda
    COMMAND rm -rf ${COVERAGE_DIR}
)