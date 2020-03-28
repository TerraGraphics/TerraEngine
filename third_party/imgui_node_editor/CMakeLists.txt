cmake_minimum_required(VERSION 3.10)
project(imgui_node_editor LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(SOURCE_FILES
        Source/crude_json.cpp
        Source/imgui_canvas.cpp
        Source/imgui_node_editor_api.cpp
        Source/imgui_node_editor.cpp
        Support/imgui_node_editor.natvis)

set(HEADER_FILES
        Include/imgui_node_editor.h
        Source/crude_json.h
        Source/imgui_bezier_math.h
        Source/imgui_bezier_math.inl
        Source/imgui_canvas.h
        Source/imgui_extra_math.h
        Source/imgui_extra_math.inl
        Source/imgui_node_editor_internal.h
        Source/imgui_node_editor_internal.inl)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CONAN_LIBS})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/Source
        ${PROJECT_SOURCE_DIR}/Include
)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
