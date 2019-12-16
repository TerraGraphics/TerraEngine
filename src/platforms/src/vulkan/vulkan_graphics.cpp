#include "vulkan/vulkan_graphics.h"

#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#include "core/common/exception.h"


VulkanGraphics::VulkanGraphics(uint32_t window, xcb_connection_t* connection)
    : m_window(window)
    , m_connection(connection) {
}

void VulkanGraphics::Create(int validationLevel) {
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
        throw EngineError("failed to initialize Vulkan");
    }

    if (!m_swapChain) {
        Diligent::SwapChainDesc scDesc;
        struct XCBInfo {
            xcb_connection_t* connection;
            uint32_t window;
        } nativeWindowHandle = {m_connection, m_window};
        engineFactoryVk->CreateSwapChainVk(m_device, contexts[0], scDesc, &nativeWindowHandle, &m_swapChain);
    }

    m_immediateContext.Attach(contexts[0]);
    auto numDeferredCtx = contexts.size() - 1;
    m_deferredContexts.resize(numDeferredCtx);
    for (uint32_t ctx = 0; ctx != numDeferredCtx; ++ctx) {
        m_deferredContexts[ctx].Attach(contexts[1 + ctx]);
    }
}
