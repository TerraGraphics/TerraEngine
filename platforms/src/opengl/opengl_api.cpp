#include "platforms/opengl/opengl_api.h"

#include <memory>
#include <vector>
#include <cstdint>
#include <stdexcept>

#include <DiligentCore/Platforms/interface/NativeWindow.h>

#include "dg/dg.h"
#include "dg/errors.h"
#include "dg/engine_factory_open_gl.h"


OpenGLAPI::OpenGLAPI(uint32_t window, Display* display) {
    m_createInfo.Window.WindowId = window;
    m_createInfo.Window.pDisplay = display;
}

void OpenGLAPI::Create(int /* validationLevel */) {
#if EXPLICITLY_LOAD_ENGINE_GL_DLL
    // Declare function pointer
    GetEngineFactoryOpenGLType GetEngineFactoryOpenGL = nullptr;
    // Load the dll and import GetEngineFactoryOpenGL() function
    LoadGraphicsEngineOpenGL(GetEngineFactoryOpenGL);
#endif

    auto* engineFactoryGL = dg::GetEngineFactoryOpenGL();
    m_engineFactory = engineFactoryGL;

    if (m_createInfo.NumDeferredContexts != 0) {
        LOG_ERROR_MESSAGE("Deferred contexts are not supported in OpenGL mode");
        m_createInfo.NumDeferredContexts = 0;
    }

    std::vector<ContextRaw> contexts;
    contexts.resize(1 + m_createInfo.NumDeferredContexts);

    dg::SwapChainDesc scDesc;
    engineFactoryGL->CreateDeviceAndSwapChainGL(m_createInfo, &m_device, contexts.data(), scDesc, &m_swapChain);
    if (!m_device) {
        throw std::runtime_error("failed to initialize OpenGL");
    }

    m_context.Attach(contexts[0]);
    auto numDeferredCtx = contexts.size() - 1;
    m_deferredContexts.resize(numDeferredCtx);
    for (uint32_t ctx = 0; ctx != numDeferredCtx; ++ctx) {
        m_deferredContexts[ctx].Attach(contexts[1 + ctx]);
    }
}
