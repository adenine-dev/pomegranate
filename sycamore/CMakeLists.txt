file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_library(sycamore ${SOURCES})
set_target_properties(sycamore PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(sycamore PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${SM_OUTPUT_DIRECTORY})

target_include_directories(sycamore PUBLIC "../vendor/SDL/include")
target_link_libraries(sycamore PUBLIC SDL2main SDL2-static)
if(MINGW OR CYGWIN)
    target_link_options(sycamore PUBLIC -lmingw32)
endif()

target_include_directories(sycamore PUBLIC "./")

target_precompile_headers(sycamore PRIVATE "pch.hpp")

target_compile_definitions(sycamore PUBLIC "$<$<PLATFORM_ID:Windows>:SM_PLATFORM_WINDOWS>")

assign_source_group(${SOURCES})
define_filename_macro(${SOURCES})
