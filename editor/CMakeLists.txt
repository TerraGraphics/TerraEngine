cmake_minimum_required(VERSION 3.10)
project(terra VERSION 0.2 LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CMAKE_SOURCE_DIR}/platforms/include
        ${CMAKE_SOURCE_DIR}/core/include
        ${CMAKE_SOURCE_DIR}/middleware/include
        ${CMAKE_SOURCE_DIR}/materials/std
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PRIVATE middleware ${CONAN_PKG_LIBS_IMGUI})

target_compile_options(${PROJECT_NAME}
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

set_common_project_properties(${PROJECT_NAME})
