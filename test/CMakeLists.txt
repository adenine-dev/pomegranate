file(GLOB_RECURSE SOURCES
    ./*.cpp
    ./*.hpp
)

enable_testing()

include(../vendor/doctest/scripts/cmake/doctest.cmake)

include(CTest)

add_executable(tests ${SOURCES})
target_link_libraries(tests PRIVATE pomegranate)

target_link_libraries(tests PRIVATE doctest::doctest)
target_include_directories(tests PUBLIC ../vendor/doctest/doctest)

doctest_discover_tests(tests)