cmake_minimum_required(VERSION 3.20.0)

project(pomegranate VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)

# Make binaries output in a dedicated directory depending on config (release, debug, etc).
set(POM_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${POM_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${POM_OUTPUT_DIRECTORY})

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
    add_compile_options(-Wall -Wextra -Werror)
endif()

# libraries
# fmt
add_subdirectory(vendor/fmt)

# sdl
set(CMAKE_MESSAGE_LOG_LEVEL "WARNING")

set(SDL_SHARED OFF)

if(MSVC) # if you're on msvc then some linking errors happen if you don't do this. idk why, should be fine lol.
    set(OPT_DEF_LIBC ON)
endif()

add_subdirectory(vendor/SDL)

set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")


add_subdirectory(pomegranate)
add_subdirectory(sandbox)