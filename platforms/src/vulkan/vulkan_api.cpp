#include "platforms/vulkan/vulkan_api.h"

#include <memory>
#include <vector>
#include <stdexcept>

#include <DiligentCore/Platforms/Linux/interface/LinuxNativeWindow.h>

#include "dg/dg.h"
#include "dg/engine_factory_vk.h"


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

    auto* engineFactoryVk = dg::GetEngineFactoryVk();
    m_engineFactory = engineFactoryVk;

    std::vector<ContextRaw> contexts;
    contexts.resize(1 + m_createInfo.NumDeferredContexts);

    engineFactoryVk->CreateDeviceAndContextsVk(m_createInfo, &m_device, contexts.data());
    if (!m_device) {
        throw std::runtime_error("failed to initialize Vulkan");
    }

    if (!m_swapChain) {
        dg::LinuxNativeWindow nativeWindowHandle;
        nativeWindowHandle.WindowId = m_window;
        nativeWindowHandle.pXCBConnection = m_connection;

        dg::SwapChainDesc scDesc;
        scDesc.ColorBufferFormat = dg::TEX_FORMAT_BGRA8_UNORM_SRGB;
        engineFactoryVk->CreateSwapChainVk(m_device, contexts[0], scDesc, nativeWindowHandle, &m_swapChain);
    }

    m_context.Attach(contexts[0]);
    auto numDeferredCtx = contexts.size() - 1;
    m_deferredContexts.resize(numDeferredCtx);
    for (uint32_t ctx = 0; ctx != numDeferredCtx; ++ctx) {
        m_deferredContexts[ctx].Attach(contexts[1 + ctx]);
    }
}
