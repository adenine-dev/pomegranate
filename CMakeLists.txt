cmake_minimum_required(VERSION 3.16)

# ========================================== Cmake things ==========================================
project(Pomegranate-Engine LANGUAGES CXX C)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_EXPORT_COMPILE_COMMANDS True)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include("target_add_binary_embed")
include("add_shader_module")
include("assign_source_group")
include("define_filename_macro")
include("add_embed_files")

set(POM_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:RelWithDebInfo>:MinSizeRel>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${POM_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${POM_OUTPUT_DIRECTORY})
set(POM_VENDOR_DIRECTORY ${CMAKE_SOURCE_DIR}/vendor)

# ============================================== SDL2 ==============================================
# Hack to disable the config logging for SDL because its annoying :p
# Can be circumvented by specifying --log-level=STATUS when running cmake.
SET(CMAKE_MESSAGE_LOG_LEVEL "WARNING")

set(SDL_SHARED OFF)
if (MSVC) # if you're on msvc then some linking errors happen if you don't do this. idk why, should be fine lol.
    SET(OPT_DEF_LIBC ON)
endif()
add_subdirectory(vendor/SDL2)

SET(CMAKE_MESSAGE_LOG_LEVEL "STATUS")

# ============================================= Vulkan =============================================
find_package(Vulkan REQUIRED)
message(STATUS ${Vulkan_INCLUDE_DIR})

# ========================================== SPIRV-Cross ===========================================
SET(SPIRV_CROSS_CLI OFF)
add_subdirectory(vendor/SPIRV-Cross)

# ======================================== Project things ==========================================
# different definitions for __FILENAME__ means its technically invalid, it should still work however
set(CMAKE_PCH_WARN_INVALID Off)

# set warnings
if (MSVC)
    add_compile_options(/W4 /wd4201)
else()
    add_compile_options(-Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-missing-braces -Werror)
endif()

# ========================================= Documentation ==========================================
# option(BUILD_DOCS "Build documentation" OFF)
add_subdirectory(docs)

# ====================================== Pomegranate Projects ======================================
add_subdirectory(pomegranate)
add_subdirectory(runtime)
add_subdirectory(sandbox)


# =========================================== IDE things ===========================================
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_target_properties(SDL2main SDL2-static PROPERTIES FOLDER vendor)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SDL2 PROPERTY FOLDER vendor)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/vendor/SPIRV-Cross PROPERTY FOLDER vendor)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT sandbox)
