file(GLOB_RECURSE SOURCES
    ./runtime.cpp
)

add_executable(examples ${SOURCES})

add_subdirectory(triangle)
add_dependencies(examples example_triangle)
target_compile_definitions(examples PRIVATE EXAMPLE_TRIANGLE_SO="$<TARGET_FILE_NAME:example_triangle>" examples)


if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
    set_target_properties(examples PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

target_link_libraries(examples PUBLIC SDL2main pomegranate)
if(MINGW OR CYGWIN)
    target_link_options(examples PUBLIC -lmingw32)
endif()

target_include_directories(examples PUBLIC ../)

assign_source_group(${SOURCES})
define_filename_macro(${SOURCES})
