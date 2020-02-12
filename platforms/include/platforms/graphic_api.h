#pragma once

#include <vector>
#include <DiligentCore/Common/interface/RefCntAutoPtr.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#pragma clang diagnostic pop


class GraphicAPI {
public:
    GraphicAPI() = default;
    virtual ~GraphicAPI();

    Diligent::RefCntAutoPtr<Diligent::IEngineFactory> GetEngineFactory() noexcept { return m_engineFactory; }
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> GetDevice() noexcept { return m_device; }
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> GetSwapChain() noexcept { return m_swapChain; }
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> GetImmediateContext() noexcept { return m_immediateContext; }
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> GetDeferredContexts() noexcept { return m_deferredContexts; }

    virtual void Create(int validationLevel = -1) = 0;
    void Destroy();

protected:
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_engineFactory;
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_device;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_swapChain;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_immediateContext;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> m_deferredContexts;
};
