cmake_minimum_required(VERSION 3.10)

include(${CMAKE_SOURCE_DIR}/build_utils.cmake)
include(${CMAKE_SOURCE_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
    BASIC_SETUP CMAKE_TARGETS
    BUILD_TYPE "Release"
    BUILD missing
    OPTIONS DiligentGraphics:development=False
    )
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_subdirectory(platforms)
add_subdirectory(core)
add_subdirectory(middleware)
add_subdirectory(editor)
