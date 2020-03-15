#include "platforms/vulkan/vulkan_api.h"

#include <memory>
#include <vector>
#include <stdexcept>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#pragma clang diagnostic pop

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wunused-parameter"
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsEngineVulkan/interface/EngineFactoryVk.h>
#pragma clang diagnostic pop

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
        Diligent::SwapChainDesc scDesc;

        Diligent::LinuxNativeWindow nativeWindowHandle;
        nativeWindowHandle.WindowId = m_window;
        nativeWindowHandle.pXCBConnection = m_connection;

        engineFactoryVk->CreateSwapChainVk(m_device, contexts[0], scDesc, nativeWindowHandle, &m_swapChain);
    }

    m_immediateContext.Attach(contexts[0]);
    auto numDeferredCtx = contexts.size() - 1;
    m_deferredContexts.resize(numDeferredCtx);
    for (uint32_t ctx = 0; ctx != numDeferredCtx; ++ctx) {
        m_deferredContexts[ctx].Attach(contexts[1 + ctx]);
    }
}
