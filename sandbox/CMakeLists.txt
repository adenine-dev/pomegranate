file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_executable(sandbox ${SOURCES})

add_dependencies(sandbox pomegranate)
target_include_directories(sandbox PUBLIC ../pomegranate)
target_link_libraries(sandbox PUBLIC pomegranate)