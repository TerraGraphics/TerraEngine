cmake_minimum_required(VERSION 3.10)
project(core VERSION 0.2 LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${CMAKE_SOURCE_DIR}/platforms/include
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PRIVATE platforms ${CONAN_PKG_LIBS_FMT} ${CONAN_PKG_LIBS_LIBUCL})
set_common_project_properties(${PROJECT_NAME})
