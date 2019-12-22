cmake_minimum_required(VERSION 3.10)
project(xcb_keysyms LANGUAGES C)

add_library(${PROJECT_NAME} STATIC ${PROJECT_SOURCE_DIR}/xcb_keysyms.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
