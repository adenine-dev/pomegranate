file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_library(pomegranate ${SOURCES})
set_target_properties(pomegranate PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(pomegranate PUBLIC ${POM_VENDOR_DIRECTORY}/SDL2/include)

target_include_directories(pomegranate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(pomegranate PUBLIC Vulkan_INCLUDE_DIR)
target_link_libraries(pomegranate PUBLIC Vulkan::Vulkan)

target_precompile_headers(pomegranate PUBLIC "pch.hpp")

target_compile_definitions(pomegranate PUBLIC "$<$<PLATFORM_ID:Windows>:POM_PLATFORM_WINDOWS>")

assign_source_group(${SOURCES})
define_filename_macro(${SOURCES})
