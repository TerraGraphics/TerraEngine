#pragma once

#include <DiligentCore/Common/interface/RefCntAutoPtr.h>


namespace Diligent {
    class IRenderDevice;
    class IDeviceContext;
}

namespace dg = Diligent;

using DevicePtr = dg::RefCntAutoPtr<dg::IRenderDevice>;
using ContextPtr = dg::RefCntAutoPtr<dg::IDeviceContext>;