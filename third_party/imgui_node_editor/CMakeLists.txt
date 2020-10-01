cmake_minimum_required(VERSION 3.10)
project(imgui_node_editor LANGUAGES CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

file(GLOB SOURCE_FILES "${PROJECT_SOURCE_DIR}/repo/*.cpp")
file(GLOB HEADER_FILES "${PROJECT_SOURCE_DIR}/repo/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/repo)
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)

# Introduce variables:
# * CMAKE_INSTALL_LIBDIR
# * CMAKE_INSTALL_INCLUDEDIR
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT ${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
        NAMESPACE ${PROJECT_NAME}::
        FILE ${PROJECT_NAME}-config.cmake)

install(FILES ${HEADER_FILES}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
