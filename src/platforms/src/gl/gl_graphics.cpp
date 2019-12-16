#include "gl/gl_graphics.h"

#include <DiligentCore/Graphics/GraphicsEngineOpenGL/interface/EngineFactoryOpenGL.h>

#include "core/common/exception.h"


GLGraphics::GLGraphics(void* display, void* nativeWindowHandle) {
    m_createInfo.pNativeWndHandle = nativeWindowHandle;
#if PLATFORM_LINUX
    m_createInfo.pDisplay = display;
#endif
}

void GLGraphics::Create(int /* validationLevel */) {
#if !PLATFORM_MACOS
    if (m_createInfo.pNativeWndHandle == nullptr) {
        throw EngineError("nativeWindowHandle for GL init is equal null");
    }
#endif

#if EXPLICITLY_LOAD_ENGINE_GL_DLL
    // Declare function pointer
    GetEngineFactoryOpenGLType GetEngineFactoryOpenGL = nullptr;
    // Load the dll and import GetEngineFactoryOpenGL() function
    LoadGraphicsEngineOpenGL(GetEngineFactoryOpenGL);
#endif

    auto* engineFactoryGL = Diligent::GetEngineFactoryOpenGL();
    m_engineFactory = engineFactoryGL;

    if (m_createInfo.NumDeferredContexts != 0) {
        LOG_ERROR_MESSAGE("Deferred contexts are not supported in OpenGL mode");
        m_createInfo.NumDeferredContexts = 0;
    }

    std::vector<Diligent::IDeviceContext*> contexts;
    contexts.resize(1 + m_createInfo.NumDeferredContexts);

    Diligent::SwapChainDesc scDesc;
    engineFactoryGL->CreateDeviceAndSwapChainGL(m_createInfo, &m_device, contexts.data(), scDesc, &m_swapChain);
    if (!m_device) {
        throw EngineError("failed to initialize OpenGL");
    }

    m_immediateContext.Attach(contexts[0]);
    auto numDeferredCtx = contexts.size() - 1;
    m_deferredContexts.resize(numDeferredCtx);
    for (uint32_t ctx = 0; ctx != numDeferredCtx; ++ctx) {
        m_deferredContexts[ctx].Attach(contexts[1 + ctx]);
    }
}
