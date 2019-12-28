#pragma once

#include <memory>

#include "core/common/dg.h"
#include "core/common/ctor.h"
#include "platforms/platforms.h"


class Application {
public:
    Application() = default;
    virtual ~Application() = default;

    virtual void Create() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;
    virtual void Destroy() = 0;
};

struct EngineDesc {
    std::shared_ptr<RenderWindow> window = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> eventHandler = nullptr;
    std::shared_ptr<Graphics> graphics = nullptr;
    std::unique_ptr<Application> application = nullptr;
    bool isVSync = true;
};

class Engine final : Fixed {
private:
    Engine() = default;
    ~Engine() = default;

public:
    static Engine& Get() noexcept {
        static Engine instance;
        return instance;
    }

    DevicePtr GetDevice() noexcept { return m_device; }
    SwapChainPtr GetSwapChain() noexcept { return m_swapChain; }
    ContextPtr GetImmediateContext() noexcept { return m_immediateContext; }
    EngineFactoryPtr GetEngineFactory() noexcept { return m_engineFactory; }
    std::shared_ptr<WindowEventsHandler> GetEventHandler() noexcept { return m_eventHandler; }

    void Create(EngineDesc&& desc);
    void Run();
    void Destroy();

private:
    std::shared_ptr<RenderWindow> m_window = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;
    std::shared_ptr<Graphics> m_graphics = nullptr;
    std::unique_ptr<Application> m_application = nullptr;

    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    ContextPtr m_immediateContext;
    EngineFactoryPtr m_engineFactory;

    bool m_isVSync = true;
};
