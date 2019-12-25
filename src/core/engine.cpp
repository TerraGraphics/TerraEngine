#include "core/engine.h"

#include "core/common/exception.h"


void Engine::Create(const std::shared_ptr<RenderWindow>& window, const std::shared_ptr<WindowEventsHandler>& eventHandler,
    const std::shared_ptr<Graphics>& graphics, std::unique_ptr<Application>&& application) {

    m_window = window;
    m_eventHandler = eventHandler;
    m_graphics = graphics;
    m_application = std::move(application);

    if (!m_window) {
        throw EngineError("window is not set");
    }
    if (!m_eventHandler) {
        throw EngineError("eventHandler is not set");
    }
    if (!m_graphics) {
        throw EngineError("graphics is not set");
    }
    if (!m_application) {
        throw EngineError("application is not set");
    }

    m_device = m_graphics->GetDevice();
    m_swapChain = m_graphics->GetSwapChain();
    m_immediateContext = m_graphics->GetImmediateContext();
    m_engineFactory = m_graphics->GetEngineFactory();

    m_application->Create();
}

void Engine::Run() {
    while (true) {
        m_window->ProcessEvents();
        if (m_window->IsWindowShouldClose()) {
            break;
        }
        m_application->Update(float(1.f / 60.f));
        m_application->Draw();
    }
}

void Engine::Destroy() {
    if (m_application) {
        m_application->Destroy();
        m_application = nullptr;
    }

    if (m_graphics) {
        m_device.Release();
        m_swapChain.Release();
        m_immediateContext.Release();
        m_engineFactory.Release();
        m_graphics->Destroy();
        m_graphics = nullptr;
    }

    if (m_window) {
        m_window->Destroy();
        m_window = nullptr;
        m_eventHandler = nullptr;
    }
}
