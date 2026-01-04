cmake_minimum_required(VERSION 3.10)
project(git_using_c C)

set(CMAKE_C_STANDARD 99)

# Output binary in project root
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Include directories
include_directories(
    components/include
    components/typedef
)

# Only non-hidden files
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR}
    "main.c"
    "components/commands/[!.]*.c"
    "components/core/[!.]*.c"
    "components/helper/[!.]*.c"
)

# Add executable
add_executable(gitc ${SOURCES})

# Link OpenSSL
find_package(OpenSSL REQUIRED)
target_link_libraries(gitc OpenSSL::Crypto)
