find_package(Doxygen REQUIRED)

if (DOXYGEN_FOUND)
    set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bin)

    set(DOXYGEN_INPUT_DIR ${PROJECT_SOURCE_DIR}/sycamore)
    set(DOXYGEN_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
    set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)
    set(DOXYFILE_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # Replace variables inside @@ with the current values
    configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

    file(MAKE_DIRECTORY ${DOXYGEN_OUTPUT_DIR})

    file(GLOB_RECURSE SYCAMORE_SOURCES
        ${CMAKE_SOURCE_DIR}/sycamore/**.hpp
        ${CMAKE_SOURCE_DIR}/sycamore/**.cpp
    )

    add_custom_target(docs
                      ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
                      DEPENDS ${SYCAMORE_SOURCES} ${DOXYFILE_IN}
                      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                      COMMENT "Generating documentation." 
                      VERBATIM)

endif()