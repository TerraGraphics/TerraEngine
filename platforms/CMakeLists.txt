cmake_minimum_required(VERSION 3.10)
project(platforms VERSION 0.1 LANGUAGES CXX)

add_definitions(-DPLATFORM_LINUX -DVULKAN_SUPPORTED)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src
)

add_subdirectory(third_party/xcb_keysyms)

target_link_libraries(${PROJECT_NAME}
PRIVATE
    xcb_keysyms
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)
