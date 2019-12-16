#pragma once

#include <vector>
#include <DiligentCore/Common/interface/RefCntAutoPtr.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>


namespace Diligent {
    class IEngineFactory;
}

class Graphics {
public:
    Graphics() = default;
    ~Graphics();

    virtual void Create(int validationLevel = -1) = 0;

    Diligent::RefCntAutoPtr<Diligent::IEngineFactory> GetEngineFactory() noexcept { return m_engineFactory; }
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> GetDevice() noexcept { return m_device; }
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> GetSwapChain() noexcept { return m_swapChain; }
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> GetImmediateContext() noexcept { return m_immediateContext; }
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> GetDeferredContexts() noexcept { return m_deferredContexts; }

protected:
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_engineFactory;
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_device;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_swapChain;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_immediateContext;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> m_deferredContexts;
};
