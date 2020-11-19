function(set_common_project_properties TARGET IWYU_FILE_NAME)
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

            # -H
            # -ftime-trace
            # -ftime-report
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

    if(${IWYU_ENABLE})
        set(IWYU_OPTIONS ${IWYU_PATH} -Xiwyu --mapping_file=${CMAKE_SOURCE_DIR}/ci/iwyu/${IWYU_FILE_NAME})
        set_property(TARGET ${TARGET} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_OPTIONS})
    endif()
endfunction()
