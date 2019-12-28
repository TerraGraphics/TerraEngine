#pragma once

#ifdef PLATFORM_LINUX

#if VULKAN_SUPPORTED
#include "platforms/linux/vulkan_window.h"
#include "platforms/vulkan/vulkan_graphics.h"
#endif

#if GL_SUPPORTED
#include "platforms/linux/gl_window.h"
#include "platforms/gl/gl_graphics.h"
#endif

#endif // PLATFORM_LINUX

#include "platforms/debug_window_handler.h"
