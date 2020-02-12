#pragma once

#include <memory>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"


class Application {
public:
    Application() = default;
    virtual ~Application() = default;

    virtual void Create() = 0;
    virtual void Update(double deltaTime) = 0;
    virtual void Draw() = 0;
    virtual void Destroy() = 0;
};

class GraphicAPI;
class RenderWindow;
class DefaultWindowEventsHandler;
struct EngineDesc {
    std::shared_ptr<RenderWindow> window = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> eventHandler = nullptr;
    std::shared_ptr<GraphicAPI> gAPI = nullptr;
    std::unique_ptr<Application> application = nullptr;
    bool isVSync = true;
};

class FileManager;
class MaterialBuilder;
class Engine final : Fixed {
private:
    Engine();
    ~Engine();

public:
    static Engine& Get() noexcept {
        static Engine instance;
        return instance;
    }

    DevicePtr& GetDevice() noexcept { return m_device; }
    SwapChainPtr& GetSwapChain() noexcept { return m_swapChain; }
    ContextPtr& GetImmediateContext() noexcept { return m_immediateContext; }
    EngineFactoryPtr& GetEngineFactory() noexcept { return m_engineFactory; }

    std::shared_ptr<RenderWindow>& GetWindow() noexcept { return m_window; }
    std::shared_ptr<DefaultWindowEventsHandler>& GetEventHandler() noexcept { return m_eventHandler; }

    std::shared_ptr<FileManager>& GetFileManager() noexcept { return m_fileManager; }
    std::shared_ptr<MaterialBuilder>& GetMaterialBuilder() noexcept { return m_materialBuilder; }

    float GetFps() const noexcept {
        return static_cast<float>(static_cast<double>(m_fpsCounter.Size()) / m_fpsCounter.Sum());
    }

    void Create(EngineDesc&& desc);
    void Run();
    void Destroy();

private:
    std::shared_ptr<RenderWindow> m_window = nullptr;
    std::shared_ptr<DefaultWindowEventsHandler> m_eventHandler = nullptr;
    std::shared_ptr<GraphicAPI> m_gAPI = nullptr;
    std::unique_ptr<Application> m_application = nullptr;

    std::shared_ptr<FileManager> m_fileManager;
    std::shared_ptr<MaterialBuilder> m_materialBuilder;

    DevicePtr m_device;
    SwapChainPtr m_swapChain;
    ContextPtr m_immediateContext;
    EngineFactoryPtr m_engineFactory;

    bool m_isVSync = true;

    CyclicalCounter<double, 120> m_fpsCounter = CyclicalCounter<double, 120>(1./60.);
};
