# Reference: https://github.com/vndmtrx/check-cmake-example

# If pkg-config is not installed on the system, then the CHECK_INSTALL_DIR var
# must be set to the install location of Check. For example, on Windows, this
# may be: C:/Program Files/check
# set(CHECK_INSTALL_DIR "C:/Program Files/check")

find_package(Check REQUIRED)

include(CheckCSourceCompiles)
include(CheckCSourceRuns)
include(CheckFunctionExists)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckSymbolExists)
include(CheckTypeSize)

enable_testing()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib/lua/)

# Object index metamethod tests
add_executable(test_object_index test_object_index.c)
target_link_libraries(test_object_index ${CHECK_LIBRARIES} pthread lua53 luastruct)
set(OBJECT_INDEX_TEST_CASES
    primitives
    objects
    unknown_field
)
foreach(test ${OBJECT_INDEX_TEST_CASES})
    add_test(NAME "object_index_${test}" COMMAND test_object_index ${test})
endforeach()

# Object newindex metamethod tests
add_executable(test_object_newindex test_object_newindex.c)
target_link_libraries(test_object_newindex ${CHECK_LIBRARIES} pthread lua53 luastruct)
set(OBJECT_NEWINDEX_TEST_CASES
    primitives
    objects
    unknown_field
)
foreach(test ${OBJECT_NEWINDEX_TEST_CASES})
    add_test(NAME "object_newindex_${test}" COMMAND test_object_newindex ${test})
endforeach()

# Array index metamethod tests
add_executable(test_array_index test_array_index.c)
target_link_libraries(test_array_index ${CHECK_LIBRARIES} pthread lua53 luastruct)
set(ARRAY_INDEX_TEST_CASES
    primitives
    objects
    out_of_bounds
)
foreach(test ${ARRAY_INDEX_TEST_CASES})
    add_test(NAME "array_index_${test}" COMMAND test_array_index ${test})
endforeach()
