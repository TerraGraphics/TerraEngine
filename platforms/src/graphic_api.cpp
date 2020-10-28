#include "platforms/graphic_api.h"

#include "dg/device.h" // IWYU pragma: keep
#include "dg/context.h"


GraphicAPI::~GraphicAPI() {
    Destroy();
}

void GraphicAPI::Destroy() {
    if (!m_context) {
        return;
    }

    m_context->Flush();
    m_swapChain.Release();
    m_deferredContexts.clear();
    m_context.Release();
    m_device.Release();
    m_engineFactory.Release();
}
