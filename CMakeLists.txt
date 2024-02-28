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
set(CMAKE_C_FLAGS_DEBUG "-g")

# Set the compiler flags for the release build
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_BUILD_TYPE Debug)

# Set the compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -pedantic -std=c99")

# Set the compiler flags for the debug build
set(CMAKE_C_FLAGS_DEBUG "-g")

# Set the compiler flags for the release build
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Create an executable for cwars_server
add_executable(cwars_server ${SERVER_SOURCE_FILES} ${COMMON_SOURCE_FILES})

# Create an executable for cwars_client
add_executable(cwars_client ${CLIENT_SOURCE_FILES} ${COMMON_SOURCE_FILES})

# Add math library
target_link_libraries(cwars_server m)
target_link_libraries(cwars_client m)
