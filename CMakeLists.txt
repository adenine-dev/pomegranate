cmake_minimum_required(VERSION 3.16)

# =================================================== Project things ===================================================
project(Sycamore LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)


# ======================================================== SDL2 ========================================================
# TODO: make sure SDL_MAIN_HANDLED doesn't break things
add_compile_definitions(SDL_MAIN_HANDLED) # allows including SDL.h in precompiled headers
# Produce a console
if (WIN32)
    if(MINGW OR CYGWIN)
        list(APPEND EXTRA_LDFLAGS "-mconsole")
    endif()
endif()

set(SDL_SHARED OFF) # static libs so it can ship as a single executable
add_subdirectory(vendor/SDL)


# ================================================ More project things =================================================
# Set the __FILENAME__ to be relative to the root directory instead of the drive with __FILE__.
if (MSVC)
    # MSVC doesn't always have __FILE__ be absolute.
    add_compile_options(/FC)
endif()
string(LENGTH ${CMAKE_SOURCE_DIR}/ SOURCE_PATH_LEN)
if (CMAKE_GENERATOR STREQUAL "Ninja" AND NOT MSVC)
    # This is dumb, but kinda has to be hardcoded because of ninja's weirdness
    # https://gitlab.kitware.com/cmake/cmake/-/issues/13894
    # If you are doing out of source builing then this will probably break.
    set(SOURCE_PATH_LEN "3")
endif()
add_compile_definitions("__FILENAME__=(&__FILE__[${SOURCE_PATH_LEN}])")

# set warnings
if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -Wno-missing-braces)
    
    # prevent clangd from complaining
    add_compile_options(-Wno-missing-braces)
endif()


# ================================================= Sycamore Projects =================================================
add_subdirectory(sycamore)
add_subdirectory(sandbox)