cmake_minimum_required(VERSION 3.16)

project(Sycamore LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

# For clang tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

# Set the __FILENAME__ to be relative to the root directory instead of the drive with __FILE__.
string(LENGTH ${CMAKE_SOURCE_DIR}/ SOURCE_PATH_LEN)
if (CMAKE_GENERATOR STREQUAL "Ninja" AND NOT MSVC)
    # This is dumb, but kinda has to be hardcoded because of ninja's weirdness
    # https://gitlab.kitware.com/cmake/cmake/-/issues/13894
    # If you are doing out of source builing then this will probably break.
    set(SOURCE_PATH_LEN "3")
endif()
add_compile_definitions("__FILENAME__=(&__FILE__[${SOURCE_PATH_LEN}])")
# MSVC doesn't always have __FILE__ be absolute.
if (MSVC)
    add_compile_options(/FC)
endif()

# set warnings
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_subdirectory(sycamore)
add_subdirectory(sandbox)