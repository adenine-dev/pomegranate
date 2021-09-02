file(GLOB_RECURSE SOURCES
    ./main.cpp
)

add_library(example_triangle SHARED ${SOURCES})

file(GLOB_RECURSE SHADER_MODULES
    # valid glslc extensions
    ./embed/*.vert
    ./embed/*.frag
    ./embed/*.comp
    ./embed/*.geom
    ./embed/*.tesc
    ./embed/*.tese
)

add_shader_modules(example_triangle ${SHADER_MODULES})

target_link_libraries(example_triangle pomegranate)
target_compile_definitions(example_triangle PRIVATE POM_CLIENT)

target_include_directories(example_triangle PUBLIC ../)

assign_source_group(${SOURCES})
define_filename_macro(${SOURCES})
