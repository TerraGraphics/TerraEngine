#include "platforms/vulkan/vulkan_api.h"

#include <memory>
#include <vector>
#include <stdexcept>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#pragma GCC diagnostic pop

#include <DiligentCore/Platforms/Linux/interface/LinuxNativeWindow.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>


VulkanAPI::VulkanAPI(uint32_t window, xcb_connection_t* connection)
    : m_window(window)
    , m_connection(connection) {
}

void VulkanAPI::Create(int validationLevel) {
#ifdef DEVELOPMENT
    m_createInfo.EnableValidation = true;
#endif
    if (validationLevel >= 1) {
        m_createInfo.EnableValidation = true;
    } else if (validationLevel == 0) {
        m_createInfo.EnableValidation = false;
    }

#if EXPLICITLY_LOAD_ENGINE_VK_DLL
    GetEngineFactoryVkType GetEngineFactoryVk = nullptr;
    // Load the dll and import GetEngineFactoryVk() function
    LoadGraphicsEngineVk(GetEngineFactoryVk);
#endif

    auto* engineFactoryVk = Diligent::GetEngineFactoryVk();
    m_engineFactory = engineFactoryVk;

    std::vector<Diligent::IDeviceContext*> contexts;
    contexts.resize(1 + m_createInfo.NumDeferredContexts);

    engineFactoryVk->CreateDeviceAndContextsVk(m_createInfo, &m_device, contexts.data());
    if (!m_device) {
        throw std::runtime_error("failed to initialize Vulkan");
    }

    if (!m_swapChain) {
        Diligent::LinuxNativeWindow nativeWindowHandle;
        nativeWindowHandle.WindowId = m_window;
        nativeWindowHandle.pXCBConnection = m_connection;

        Diligent::SwapChainDesc scDesc;
        scDesc.ColorBufferFormat = Diligent::TEX_FORMAT_BGRA8_UNORM_SRGB;
        engineFactoryVk->CreateSwapChainVk(m_device, contexts[0], scDesc, nativeWindowHandle, &m_swapChain);
    }

    m_context.Attach(contexts[0]);
    auto numDeferredCtx = contexts.size() - 1;
    m_deferredContexts.resize(numDeferredCtx);
    for (uint32_t ctx = 0; ctx != numDeferredCtx; ++ctx) {
        m_deferredContexts[ctx].Attach(contexts[1 + ctx]);
    }
}
