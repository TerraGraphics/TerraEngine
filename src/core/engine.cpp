#include "core/engine.h"

#include "core/common/path.h"
#include "core/common/timer.h"
#include "platforms/platforms.h"
#include "core/common/exception.h"


Engine::Engine() {
    m_fileManager = std::make_shared<FileManager>();
}

Engine::~Engine() {

}

void Engine::Create(EngineDesc&& desc) {
    m_window = desc.window;
    m_eventHandler = desc.eventHandler;
    m_graphics = desc.graphics;
    m_application = std::move(desc.application);
    m_isVSync = desc.isVSync;

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
    Timer timer;
    timer.Start();

    while (true) {
        m_window->ProcessEvents();
        if (m_eventHandler->IsWindowShouldClose()) {
            break;
        }

        uint32_t windowWidth;
        uint32_t windowHeight;
        if (m_eventHandler->GetWindowSize(windowWidth, windowHeight)) {
            auto swapWidth = m_swapChain->GetDesc().Width;
            auto swapHeight = m_swapChain->GetDesc().Height;

            if (((windowWidth != swapWidth) || (windowHeight != swapHeight)) && (windowWidth != 0) && (windowHeight != 0)) {
                m_swapChain->Resize(windowWidth, swapHeight);
            }
        }

        auto dt = timer.TimePoint();
        m_application->Update(dt);

        m_application->Draw();
        m_swapChain->Present(m_isVSync ? 1 : 0);

        m_fpsCounter.Add(dt);
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
