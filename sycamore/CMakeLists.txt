file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_library(sycamore ${SOURCES})
set_target_properties(sycamore PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(sycamore PUBLIC ${SDL2_INCLUDE_DIR})
target_link_libraries(sycamore SDL2-static)

target_include_directories(sycamore PUBLIC "./")

target_precompile_headers(sycamore PRIVATE "pch.hpp")

target_compile_definitions(sycamore PUBLIC "$<$<PLATFORM_ID:Windows>:SM_PLATFORM_WINDOWS>")

