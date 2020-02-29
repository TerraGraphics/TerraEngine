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
target_link_libraries(${PROJECT_NAME} PRIVATE core ${CONAN_PKG_LIBS_SPDLOG} ${CONAN_PKG_LIBS_IMGUI})
if(${IWYU_ENABLE})
    set(IWYU_OPTIONS ${IWYU_PATH} -Xiwyu --mapping_file=${CMAKE_SOURCE_DIR}/ci/iwyu/middleware.imp)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_INCLUDE_WHAT_YOU_USE ${IWYU_OPTIONS})
endif()
set_common_project_properties(${PROJECT_NAME})
