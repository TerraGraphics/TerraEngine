#include "core/engine.h"

#include <cstdint>
#include <utility>

#include "dg/device.h"
#include "dg/context.h"
#include "core/path/path.h"
#include "core/common/timer.h"
#include "platforms/platforms.h"
#include "core/common/exception.h"
#include "core/material/vdecl_storage.h"
#include "core/material/texture_manager.h"
#include "core/material/material_builder.h"


Engine::Engine()
    : m_fileManager(std::make_shared<FileManager>())
    , m_vDeclStorage(std::make_shared<VDeclStorage>()) {

}

Engine::~Engine() {

}

void Engine::Create(EngineDesc&& desc) {
    m_window = desc.window;
    m_eventHandler = desc.eventHandler;
    m_gAPI = desc.gAPI;
    m_application = std::move(desc.application);
    m_isVSync = desc.isVSync;

    if (!m_window) {
        throw EngineError("window is not set");
    }
    if (!m_eventHandler) {
        throw EngineError("eventHandler is not set");
    }
    if (!m_gAPI) {
        throw EngineError("graphics is not set");
    }
    if (!m_application) {
        throw EngineError("application is not set");
    }

    m_device = m_gAPI->GetDevice();
    m_swapChain = m_gAPI->GetSwapChain();
    m_context = m_gAPI->GetContext();
    m_engineFactory = m_gAPI->GetEngineFactory();
    m_textureManager = std::make_shared<TextureManager>(m_device);
    m_materialBuilder = std::make_shared<MaterialBuilder>(m_device, m_context, m_swapChain, m_engineFactory, m_vDeclStorage);

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

    if (m_gAPI) {
        m_device.Release();
        m_swapChain.Release();
        m_context.Release();
        m_engineFactory.Release();
        m_gAPI->Destroy();
        m_gAPI = nullptr;
    }

    if (m_window) {
        m_window->Destroy();
        m_window = nullptr;
        m_eventHandler = nullptr;
    }
}
