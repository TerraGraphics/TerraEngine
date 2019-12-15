#pragma once

#include <vector>
#include <DiligentCore/Common/interface/RefCntAutoPtr.h>


namespace Diligent {
    class ISwapChain;
    class IRenderDevice;
    class IEngineFactory;
    class IDeviceContext;
}

class Graphics {
public:
    Graphics() = default;
    ~Graphics() = default;

protected:
    Diligent::RefCntAutoPtr<Diligent::IEngineFactory> m_engineFactory;
    Diligent::RefCntAutoPtr<Diligent::IRenderDevice> m_device;
    Diligent::RefCntAutoPtr<Diligent::ISwapChain> m_swapChain;
    Diligent::RefCntAutoPtr<Diligent::IDeviceContext> m_immediateContext;
    std::vector<Diligent::RefCntAutoPtr<Diligent::IDeviceContext>> m_deferredContexts;
};
