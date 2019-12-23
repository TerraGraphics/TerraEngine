#include "platforms/graphics.h"


Graphics::~Graphics() {
    Destroy();
}

void Graphics::Destroy() {
    if (m_immediateContext) {
        m_immediateContext->Flush();
    }
    m_deferredContexts.clear();
    m_immediateContext.Release();
    m_swapChain.Release();
    m_device.Release();
    m_engineFactory.Release();
}
