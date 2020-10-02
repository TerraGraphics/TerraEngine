cmake_minimum_required(VERSION 3.10)
project(DiligentGraphics)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
add_compile_options(-fms-extensions)

add_subdirectory(DiligentCore)
add_subdirectory(DiligentTools)
