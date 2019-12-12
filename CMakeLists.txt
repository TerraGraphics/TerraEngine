cmake_minimum_required(VERSION 3.10)

add_subdirectory(third_party/xcb_keysyms)

project(terra VERSION 0.1 LANGUAGES CXX)
include(${CMAKE_SOURCE_DIR}/conan.cmake)
conan_cmake_run(CONANFILE conanfile.txt
                BASIC_SETUP CMAKE_TARGETS
                BUILD_TYPE "Release"
                BUILD missing)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_definitions(-DPLATFORM_LINUX -DVULKAN_SUPPORTED)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

file(GLOB_RECURSE SOURCE_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "src" "src/SampleBase/include" "assets")

target_link_libraries(${PROJECT_NAME}
PRIVATE
    GL
    X11
    xcb

    xcb_keysyms

    Diligent-Common
    Diligent-GraphicsTools
    Diligent-TextureLoader
    Diligent-Imgui

    GraphicsEngineVk
    GraphicsEngineOpenGL

    LibJpeg
    LibPng
    LibTiff
    ZLib
    DiligentCore

    ${CONAN_PKG_LIBS_FMT}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
