cmake_minimum_required(VERSION 3.10)
project(middleware VERSION 0.2 LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/platforms/include
        ${CMAKE_SOURCE_DIR}/core/include
        ${CMAKE_SOURCE_DIR}/materials/std
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PRIVATE platforms core ${CONAN_PKG_LIBS_FMT} ${CONAN_PKG_LIBS_LIBUCL} ${CONAN_PKG_LIBS_SPDLOG} ${CONAN_PKG_LIBS_IMGUI})

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
