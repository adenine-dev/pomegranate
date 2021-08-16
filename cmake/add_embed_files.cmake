function(add_embed_files TARGET)    
    foreach(_source IN ITEMS ${ARGN})

        get_filename_component(outdir ${_source} DIRECTORY)
        target_add_binary_embed(${TARGET} ${_source} OUTDIR ${outdir})
    endforeach()

endfunction(add_embed_files)
