function(set_common_project_properties TARGET)
    set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
    )

    target_compile_definitions(${TARGET}
    PRIVATE
        GL_SUPPORTED=$<BOOL:${GL_SUPPORTED}>
        VULKAN_SUPPORTED=$<BOOL:${VULKAN_SUPPORTED}>
    )

    if(PLATFORM_LINUX)
        target_compile_definitions(${TARGET} PRIVATE PLATFORM_LINUX=1)
    endif()
endfunction()
