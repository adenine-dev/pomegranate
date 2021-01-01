cmake_minimum_required(VERSION 3.16)

project(Sycamore LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

add_subdirectory(sycamore)
add_subdirectory(sandbox)