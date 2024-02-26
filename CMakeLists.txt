cmake_minimum_required(VERSION 3.10)

project(CWars)

# Add the include directory to the include path
include_directories(include)

# Set the source directory
set(SOURCE_DIR src)

# Add all source files in the src directory to the SOURCE_FILES variable
file(GLOB SOURCE_FILES "${SOURCE_DIR}/*.c")

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Set the build type to debug
set(CMAKE_BUILD_TYPE Debug)

# Set the compiler flags
set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -pedantic -std=c99")

# Set the compiler flags for the debug build
set(CMAKE_C_FLAGS_DEBUG "-g")

# Set the compiler flags for the release build
set(CMAKE_C_FLAGS_RELEASE "-O3")

# Set the output directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Create an executable
add_executable(cwars ${SOURCE_FILES})

# Add math library
target_link_libraries(cwars m ssl crypto)
