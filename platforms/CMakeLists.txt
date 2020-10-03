cmake_minimum_required(VERSION 3.10)
project(platforms VERSION 0.4 LANGUAGES CXX)

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

list(APPEND LIBS DiligentTools DiligentCore)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/tp
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS})
set_common_project_properties(${PROJECT_NAME} platform.imp)
