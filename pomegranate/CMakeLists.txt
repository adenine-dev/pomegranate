file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

add_library(pomegranate STATIC ${SOURCES})

target_link_libraries(pomegranate fmt::fmt)

set_target_properties(pomegranate PROPERTIES LINKER_LANGUAGE CXX)

target_include_directories(pomegranate PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(pomegranate PUBLIC vendor/fmt/include)

target_compile_definitions(pomegranate
    PUBLIC "$<$<PLATFORM_ID:Windows>:POM_PLATFORM_WINDOWS>")
