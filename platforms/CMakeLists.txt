cmake_minimum_required(VERSION 3.10)
project(platforms VERSION 0.2 LANGUAGES CXX)

include(platforms_check.cmake)

set(LIBS)
set(SOURCE_FILES
    src/debug_window_handler.cpp
    src/default_window_handler.cpp
    src/graphic_api.cpp
    src/window_events.cpp
    src/window.cpp
)

if(PLATFORM_LINUX)
    list(APPEND LIBS X11)
    list(APPEND SOURCE_FILES src/linux/x11_key_map.cpp)
    list(APPEND SOURCE_FILES src/linux/x11_input_handler.cpp)

    if(GL_SUPPORTED)
        list(APPEND SOURCE_FILES src/opengl/opengl_api.cpp)
        list(APPEND SOURCE_FILES src/linux/gl_window.cpp)
        list(APPEND LIBS GL Xcursor GraphicsEngineOpenGL)
    endif()

    if(VULKAN_SUPPORTED)
        list(APPEND SOURCE_FILES src/vulkan/vulkan_api.cpp)
        list(APPEND SOURCE_FILES src/linux/vulkan_window.cpp)
        list(APPEND LIBS xcb xcb-keysyms xcb-cursor X11-xcb GraphicsEngineVk)
    endif()
endif()

list(APPEND LIBS
    Diligent-Common
    Diligent-GraphicsTools
    Diligent-TextureLoader
    LibJpeg
    LibPng
    LibTiff
    ZLib
    DiligentCore
)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})

target_compile_options(${PROJECT_NAME}
    PRIVATE
        -Werror

        -Wall
        -Wextra
        -Wpedantic

        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wctor-dtor-privacy
        -Wenum-compare
        -Wfloat-equal
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wsign-conversion
        -Wsign-promo

        # Diligent
        -Wno-non-virtual-dtor
        -Wno-float-equal
        -Wno-unused-parameter
        -Wno-gnu-zero-variadic-macro-arguments
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

target_compile_definitions(${PROJECT_NAME}
PUBLIC
    GL_SUPPORTED=$<BOOL:${GL_SUPPORTED}>
    VULKAN_SUPPORTED=$<BOOL:${VULKAN_SUPPORTED}>
)

if(PLATFORM_LINUX)
    target_compile_definitions(${PROJECT_NAME} PUBLIC PLATFORM_LINUX=1)
endif()
