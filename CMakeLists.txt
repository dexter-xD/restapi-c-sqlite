cmake_minimum_required(VERSION 3.10)
project(todo_rest_api C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Set build type if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Add compiler warnings
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")
endif()

# Find required packages
find_package(CURL REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JANSSON REQUIRED jansson)

# Find libmicrohttpd
find_path(MICROHTTPD_INCLUDE_DIR microhttpd.h)
find_library(MICROHTTPD_LIBRARY microhttpd)

if(NOT MICROHTTPD_INCLUDE_DIR OR NOT MICROHTTPD_LIBRARY)
    message(FATAL_ERROR "libmicrohttpd not found. Please install libmicrohttpd-dev package.")
endif()

# Add subdirectories
add_subdirectory(src)
add_subdirectory(tests)

# Enable testing
enable_testing() 