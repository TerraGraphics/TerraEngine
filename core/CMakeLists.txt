cmake_minimum_required(VERSION 3.10)
project(core VERSION 0.3 LANGUAGES CXX)

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
if(${IWYU_ENABLE})
    set(IWYU_OPTIONS ${IWYU_PATH} -Xiwyu --mapping_file=${CMAKE_SOURCE_DIR}/ci/iwyu/core.imp)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_OPTIONS})
endif()
set_common_project_properties(${PROJECT_NAME})


file(GLOB_RECURSE SOURCE_TEST_FILES "${PROJECT_SOURCE_DIR}/test/*.cpp")

set(PROJECT_TEST_NAME ${PROJECT_NAME}_test)
add_executable(${PROJECT_TEST_NAME} ${SOURCE_TEST_FILES})
target_include_directories(${PROJECT_TEST_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/platforms/include
)
target_link_libraries(${PROJECT_TEST_NAME} PRIVATE ${PROJECT_NAME} ${CONAN_PKG_LIBS_GTEST})
if(${IWYU_ENABLE})
    set_property(TARGET ${PROJECT_TEST_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_OPTIONS})
endif()
set_common_project_properties(${PROJECT_TEST_NAME})
