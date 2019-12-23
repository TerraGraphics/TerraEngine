set(PLATFORM_LINUX FALSE CACHE INTERNAL "")

set(GL_SUPPORTED FALSE CACHE INTERNAL "GL is not spported")
set(VULKAN_SUPPORTED FALSE CACHE INTERNAL "Vulkan is not spported")

if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(ARCH 64 CACHE INTERNAL "64-bit architecture")
else()
    set(ARCH 32 CACHE INTERNAL "32-bit architecture")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
    set(PLATFORM_LINUX TRUE CACHE INTERNAL "Target platform: Linux")
    message("Target Platform: Linux")
else()
    message(FATAL_ERROR "Unsupported platform")
endif()

if(PLATFORM_LINUX)
    set(GL_SUPPORTED TRUE CACHE INTERNAL "OpenGL is supported on Linux platform")
    if(${ARCH} EQUAL 64)
        set(VULKAN_SUPPORTED TRUE CACHE INTERNAL "Vulkan is supported on Linux64 platform")
    endif()
else()
    message(FATAL_ERROR "No PLATFORM_XXX variable defined. Make sure that 'DiligentCore' folder is processed first")
endif()

option(DILIGENT_NO_OPENGL "Disable OpenGL/GLES backend" OFF)
option(DILIGENT_NO_VULKAN "Disable Vulkan backend" OFF)

if(${DILIGENT_NO_OPENGL})
    set(GL_SUPPORTED FALSE CACHE INTERNAL "OpenGL backend is forcibly disabled")
endif()
if(${DILIGENT_NO_VULKAN})
    set(VULKAN_SUPPORTED FALSE CACHE INTERNAL "Vulkan backend is forcibly disabled")
endif()

if(NOT (${GL_SUPPORTED} OR ${VULKAN_SUPPORTED}))
    message(FATAL_ERROR "No rendering backends are select to build")
endif()

message("GL_SUPPORTED: " ${GL_SUPPORTED})
message("VULKAN_SUPPORTED: " ${VULKAN_SUPPORTED})
