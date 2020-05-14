#pragma once

#include <vector>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#if defined(__clang__)
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#endif
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#pragma GCC diagnostic pop


namespace Diligent {
    struct ISwapChain;
    struct IRenderDevice;
    struct IEngineFactory;
    struct IDeviceContext;
}
class GraphicAPI {
public:
    GraphicAPI() = default;
    virtual ~GraphicAPI();

    Diligent::RefCntAutoPtr<Diligent::IRenderDevice>& GetDevice() noexcept { return m_device; }
    Diligent::RefCntAutoPtr<Diligent::ISwapChain>& GetSwapChain() noexcept { return m_swapChain; }
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory>& GetEngineFactory() noexcept { return m_engineFactory; }
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext>& GetContext() noexcept { return m_context; }
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>>& GetDeferredContexts() noexcept { return m_deferredContexts; }

    virtual void Create(int validationLevel = -1) = 0;
    void Destroy();

protected:
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_device;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_swapChain;
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_engineFactory;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_context;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> m_deferredContexts;
};
