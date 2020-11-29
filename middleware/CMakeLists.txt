cmake_minimum_required(VERSION 3.10)
project(middleware VERSION 0.4 LANGUAGES CXX)

file(GLOB_RECURSE SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/materials/std
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(${PROJECT_NAME} PUBLIC core PRIVATE ${CONAN_PKG_LIBS_SPDLOG} ${CONAN_PKG_LIBS_IMGUI} ${CONAN_PKG_LIBS_CPGF})
set_common_project_properties(${PROJECT_NAME} middleware.imp)


file(GLOB_RECURSE SOURCE_TEST_FILES "${PROJECT_SOURCE_DIR}/test/*.cpp")

set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
add_executable(${PROJECT_TEST_NAME} ${SOURCE_TEST_FILES})
target_include_directories(${PROJECT_TEST_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_TEST_NAME} PRIVATE pthread ${PROJECT_NAME} ${CONAN_PKG_LIBS_GTEST})
set_common_project_properties(${PROJECT_TEST_NAME} middleware.imp)
