cmake_minimum_required(VERSION 3.10)
project(imgui_node_editor LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/imgui-node-editor/*.cpp")
file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/imgui-node-editor/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
        ${PROJECT_SOURCE_DIR}/imgui-node-editor
)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        PERMISSIONS OWNER_WRITE OWNER_READ GROUP_READ WORLD_READ)
