cmake_minimum_required(VERSION 3.10)
project(terra VERSION 0.4 LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PUBLIC middleware PRIVATE pthread ${CONAN_PKG_LIBS_IMGUI} ${CONAN_PKG_LIBS_IMGUI_NODE_EDITOR})
set_common_project_properties(${PROJECT_NAME} middleware.imp)
