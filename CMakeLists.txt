cmake_minimum_required(VERSION 3.20.0)

project(pomegranate VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

# Make binaries output in a dedicated directory depending on config (release, debug, etc).
set(POM_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${POM_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${POM_OUTPUT_DIRECTORY})

add_executable(pomegranate main.cpp)

# generate compile_commands.json for clang stuffs
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include(CheckIPOSupported)
check_ipo_supported(RESULT result OUTPUT output)

if(result)
    set(INTERPROCEDURAL_OPTIMIZATION TRUE)
else()
    message(WARNING "IPO is not supported: ${output}")
endif()

if(MSVC)
    add_compile_options(/W4 /wd4201)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-missing-braces -Werror)
endif()
