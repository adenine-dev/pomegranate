function(target_add_binary_embed target filename)
    get_filename_component(base_filename ${filename} NAME)

    message("all arguments: ${ARGV}")

    if (DEFINED ARGV2)
        message(${ARGV2})
        set(pathname ${ARGV2})
    else()
        get_filename_component(pathname ${filename} DIRECTORY)
    endif()

    string(MAKE_C_IDENTIFIER ${base_filename} c_name)

    add_custom_command(
        OUTPUT ${pathname}/${c_name}.hpp ${pathname}/${c_name}.cpp
        COMMAND ${CMAKE_COMMAND} -DRUN_CREATE_BINARY_EMBED=1 -DGENERATE_FILENAME=${filename} -DGENERATE_FILEPATH=${pathname} -P ${CMAKE_SOURCE_DIR}/cmake/target_add_binary_embed.cmake
        MAIN_DEPENDENCY ${filename}
        VERBATIM
    )

    target_sources(${target} PRIVATE ${pathname}/${c_name}.hpp ${pathname}/${c_name}.cpp)

    set_source_files_properties(${pathname}/${c_name}.hpp PROPERTIES GENERATED TRUE)
    set_source_files_properties(${pathname}/${c_name}.cpp PROPERTIES GENERATED TRUE)
endfunction(target_add_binary_embed)

# you should use nothing below this.
function(create_binary_embed filename pathname)
    get_filename_component(base_filename ${filename} NAME)
    string(MAKE_C_IDENTIFIER ${base_filename} c_name)

    file(READ ${filename} content HEX)
    string(REGEX MATCHALL "([A-Fa-f0-9][A-Fa-f0-9])" SEPARATED_HEX ${content})
    set(counter 0)
    foreach (hex IN LISTS SEPARATED_HEX)
        string(APPEND output_c "0x${hex}, ")
        math(EXPR counter "${counter}+1")
        if (counter GREATER 16)
            string(APPEND output_c "\n    ")
            set(counter 0)
        endif ()
    endforeach ()

    set(output_c "// This file was automatically generated from \"${filename}\" and should not be manually modified.

#include \"${c_name}.hpp\"

const unsigned ${c_name}_data[] = {
    ${output_c}
}\;
const size_t ${c_name}_size = sizeof(${c_name}_data)\;")

    set(output_h "// This file was automatically generated from ${filename} and should not be manually modified.

#pragma once
#include <cstdint>

extern const unsigned ${c_name}_data[]\;
extern const size_t ${c_name}_size\;")

    file(WRITE ${pathname}/${c_name}.cpp ${output_c})
    file(WRITE ${pathname}/${c_name}.hpp ${output_h})

endfunction(create_binary_embed)


if (RUN_CREATE_BINARY_EMBED)
    create_binary_embed(${GENERATE_FILENAME} ${GENERATE_FILEPATH})
endif ()
