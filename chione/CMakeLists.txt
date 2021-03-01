file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_library(chione ${SOURCES})
set_target_properties(chione PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(chione PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CE_OUTPUT_DIRECTORY})

target_include_directories(chione PUBLIC ${CE_VENDOR_DIRECTORY}/SDL2/include)
target_link_libraries(chione PUBLIC SDL2main SDL2-static)
if(MINGW OR CYGWIN)
    target_link_options(chione PUBLIC -lmingw32)
endif()

target_include_directories(chione PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(chione PUBLIC Vulkan_INCLUDE_DIR)
target_link_libraries(chione PUBLIC Vulkan::Vulkan)

target_precompile_headers(chione PUBLIC "pch.hpp")

target_compile_definitions(chione PUBLIC "$<$<PLATFORM_ID:Windows>:CE_PLATFORM_WINDOWS>")

assign_source_group(${SOURCES})
define_filename_macro(${SOURCES})
