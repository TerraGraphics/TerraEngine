#pragma once

#include <DiligentCore/Common/interface/RefCntAutoPtr.h>


namespace Diligent {
    class IEngineFactory;
    class IRenderDevice;
    class IDeviceContext;
    class ISwapChain;
}

namespace dg = Diligent;

using EngineFactoryPtr = dg::RefCntAutoPtr<dg::IEngineFactory>;
using DevicePtr = dg::RefCntAutoPtr<dg::IRenderDevice>;
using ContextPtr = dg::RefCntAutoPtr<dg::IDeviceContext>;
using SwapChainPtr = dg::RefCntAutoPtr<dg::ISwapChain>;
