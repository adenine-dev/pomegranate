file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_library(pomegranate STATIC ${SOURCES})

set_target_properties(pomegranate PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(pomegranate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
