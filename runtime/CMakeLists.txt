file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_executable(pom_runtime ${SOURCES})

add_dependencies(pom_runtime sandbox)
target_compile_definitions(pom_runtime PRIVATE CLIENT_SO_FILE="$<TARGET_FILE_NAME:sandbox>" POM_RUNTIME)
set_target_properties(pom_runtime PROPERTIES WIN32_EXECUTABLE TRUE)

target_link_libraries(pom_runtime PUBLIC SDL2-static SDL2main pomegranate)
if(MINGW OR CYGWIN)
    target_link_options(pom_runtime PUBLIC -lmingw32)
endif()

# set_target_properties(pom_runtime PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${POM_OUTPUT_DIRECTORY})

target_include_directories(pom_runtime PUBLIC ../)

assign_source_group(${SOURCES})
define_filename_macro(${SOURCES})
