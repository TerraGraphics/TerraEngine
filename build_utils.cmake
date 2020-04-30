function(set_common_project_properties TARGET)
    target_compile_options(${TARGET}
        PRIVATE
            -Werror

            -Wall
            -Wextra
            -Wpedantic

            -Wcast-qual
            -Wcast-align
            -Wsign-promo
            -Wconversion
            -Wfloat-equal
            -Wenum-compare
            -Wold-style-cast
            -Wredundant-decls
            -Wsign-conversion
            -Wctor-dtor-privacy
            -Woverloaded-virtual
    )

    set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 20
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
