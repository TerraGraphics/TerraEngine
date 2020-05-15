#include "platforms/graphic_api.h"

#pragma GCC diagnostic push
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
#pragma GCC diagnostic ignored "-Wfloat-equal"
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#pragma GCC diagnostic pop
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>


GraphicAPI::~GraphicAPI() {
    Destroy();
}

void GraphicAPI::Destroy() {
    if (m_context.RawPtr() != nullptr) {
        m_context->Flush();
    }
    m_deferredContexts.clear();
    m_context.Release();
    m_swapChain.Release();
    m_device.Release();
    m_engineFactory.Release();
}
