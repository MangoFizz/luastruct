# SPDX-License-Identifier: GPL-3.0-only

cmake_minimum_required(VERSION 3.18)

project(luacstruct C)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable -Werror=unused-value")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-missing-field-initializers -Wno-format")

enable_testing()
add_subdirectory(tests/)

include(lib/lua/lua.cmake)

include_directories(lib/lua)

add_library(luastruct STATIC
    src/struct.c
    src/debug.c
    src/array.c
    src/object.c
)
