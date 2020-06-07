#pragma once

#include <vector>

#include "dg/dg.h"


class GraphicAPI {
public:
    GraphicAPI() = default;
    virtual ~GraphicAPI();

    DevicePtr& GetDevice() noexcept { return m_device; }
    SwapChainPtr& GetSwapChain() noexcept { return m_swapChain; }
    EngineFactoryPtr& GetEngineFactory() noexcept { return m_engineFactory; }
    ContextPtr& GetContext() noexcept { return m_context; }
    std::vector<ContextPtr>& GetDeferredContexts() noexcept { return m_deferredContexts; }

    virtual void Create(int validationLevel = -1) = 0;
    void Destroy();

protected:
    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    EngineFactoryPtr m_engineFactory;
    ContextPtr m_context;
    std::vector<ContextPtr> m_deferredContexts;
};
