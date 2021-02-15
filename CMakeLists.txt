cmake_minimum_required(VERSION 3.16)

# ==================================================== Cmake things ====================================================
project(Chione LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("assign_source_group")
include("define_filename_macro")

set(CE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${GENERATOR_NAME}-$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>)
set(CE_VENDOR_DIRECTORY ${CMAKE_SOURCE_DIR}/vendor)

# ======================================================== SDL2 ========================================================
# Produce a console in debug mode
if (WIN32)
    if(MINGW OR CYGWIN)
        list(APPEND EXTRA_LDFLAGS "$<$<CONFIG:Debug>:-mconsole>")
    endif()
endif()

if (MSVC)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG /SUBSYSTEM:CONSOLE)
endif()

# Hack to disable the config logging for SDL because its annoying :p
# Can be circumvented by specifying --log-level=STATUS when running cmake.
SET(CMAKE_MESSAGE_LOG_LEVEL "WARNING")

set(SDL_SHARED OFF) # static libs so it can ship as a single executable
add_subdirectory(vendor/SDL2)

SET(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

# ================================================== Project things ====================================================
# different definitions for __FILENAME__ means its technically invalid, it should still work however
set(CMAKE_PCH_WARN_INVALID Off)

# set warnings
if (MSVC)
    add_compile_options(/W4 /WX /wd4201)
else()
    add_compile_options(-Wall -Wextra -Wno-missing-braces -Werror)
endif()

# =================================================== Documentation ====================================================
# option(BUILD_DOCS "Build documentation" OFF)
add_subdirectory(docs)

# =================================================== Chione Projects ==================================================
add_subdirectory(chione)
add_subdirectory(sandbox)


# ===================================================== IDE things =====================================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(SDL2main SDL2-static PROPERTIES FOLDER vendor)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2 PROPERTY FOLDER vendor)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sandbox)
