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

# Create an executable for cwars_server
add_executable(cwars_server ${SERVER_SOURCE_FILES})

target_link_libraries(cwars_server
    m
    cwars_common
)

# Create an executable for cwars_client
add_executable(cwars_client ${CLIENT_SOURCE_FILES})

target_link_libraries(cwars_client
    m
    cwars_common
)

# Add CMocka library
find_package(CMocka REQUIRED)

enable_testing()

# Add your test source file
add_executable(test_linked_list 
    test/linked_list/test_linked_list.c
    src/common/linked_list.c
)

# Link your test executable with the CMocka library
target_link_libraries(test_linked_list ${CMOCKA_LIBRARIES})

# Add the test to CTest's test list
add_test(NAME test_linked_list COMMAND test_linked_list)