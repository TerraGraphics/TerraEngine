cmake_minimum_required(VERSION 3.10)
project(terra_engine VERSION 0.4)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT CMAKE_CXX_COMPILER_VERSION)
    message(FATAL_ERROR "Could not find CMAKE_CXX_COMPILER_VERSION")
endif()

option(TERRA_IWYU_ENABLE "include-what-you-use enable" OFF)
set(IWYU_ENABLE FALSE CACHE INTERNAL "include-what-you-use enable")
if(${TERRA_IWYU_ENABLE})
    set(IWYU_ENABLE TRUE CACHE INTERNAL "include-what-you-use enable")
endif()

if(${IWYU_ENABLE})
  find_program(IWYU_PATH NAMES include-what-you-use iwyu)
  if(NOT IWYU_PATH)
    message(FATAL_ERROR "Could not find the program include-what-you-use")
  endif()
endif()

# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks="cppcoreguidelines-*")

include(${CMAKE_SOURCE_DIR}/build_utils.cmake)
include(${CMAKE_SOURCE_DIR}/conan.cmake)
set(CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR};${CMAKE_MODULE_PATH}")

conan_add_remote(NAME bincrafters
        URL https://api.bintray.com/conan/bincrafters/public-conan
        INDEX 1
        COMMAND conan)
conan_add_remote(NAME terragraphics
        URL https://api.bintray.com/conan/terragraphics/conan-packages
        INDEX 1
        COMMAND conan)

conan_cmake_run(CONANFILE conanfile.txt
    BUILD_TYPE "Release"
    BUILD missing
    OPTIONS diligent_graphics:development=False
    SETTINGS diligent_graphics:build_type=${CMAKE_BUILD_TYPE}
    )
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

message("IWYU_ENABLE: " ${IWYU_ENABLE})
message("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})
message("CMAKE_CXX_COMPILER_ID: " ${CMAKE_CXX_COMPILER_ID})
message("CMAKE_CXX_COMPILER_VERSION: " ${CMAKE_CXX_COMPILER_VERSION})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

find_package(fmt REQUIRED)
find_package(cpgf REQUIRED)
find_package(GTest REQUIRED)
find_package(imgui REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(spdlog REQUIRED)
find_package(libucl REQUIRED)
find_package(fastnoise REQUIRED)
find_package(imgui_node_editor REQUIRED)
find_package(diligent_graphics REQUIRED)
find_package(boost_preprocessor REQUIRED)

add_subdirectory(platforms)
add_subdirectory(core)
add_subdirectory(middleware)
add_subdirectory(editor)
