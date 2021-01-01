file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_library(sycamore ${SOURCES})
set_target_properties(sycamore PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(sycamore PUBLIC "./")
