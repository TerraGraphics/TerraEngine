#include "platforms/graphic_api.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#pragma clang diagnostic ignored "-Wunused-parameter"
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#pragma clang diagnostic pop
#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>


GraphicAPI::~GraphicAPI() {
    Destroy();
}

void GraphicAPI::Destroy() {
    if (m_immediateContext) {
        m_immediateContext->Flush();
    }
    m_deferredContexts.clear();
    m_immediateContext.Release();
    m_swapChain.Release();
    m_device.Release();
    m_engineFactory.Release();
}
