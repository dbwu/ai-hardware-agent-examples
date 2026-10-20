function(goldieos_component_sdk target backend)
    target_sources(${target} PRIVATE "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${backend}/sdk_platform.c")
endfunction()
