#include <DiligentCore/Primitives/interface/Errors.h>

#include "editor.h"
#include "core/engine.h"
#include "platforms/platforms_supported.h"


static WindowDesc GetWindowSettings() {
    WindowDesc desc;
    desc.positionType = WindowDesc::Position::CenterOfScreen;

    desc.sizeType = WindowDesc::Size::PercentOfScreen;
    desc.widthPercent = 80.f;
    desc.heightPercent = 80.f;

    desc.minWidth = 320;
    desc.minHeight = 240;

    desc.name = "terra";

    return desc;
}

static EngineDesc GetEngineSettings() {
    EngineDesc desc;
    desc.application = std::make_unique<Editor>();
    desc.isVSync = true;

    // desc.eventHandler = std::make_shared<DebugWindowEventsHandler>();
    desc.eventHandler = std::make_shared<DefaultWindowEventsHandler>();

    return desc;
}

static void CreatePlatform(bool useOpenGL, WindowDesc windowDesc, EngineDesc& engineDesc) {
#ifdef PLATFORM_LINUX

if (!useOpenGL) {
#if VULKAN_SUPPORTED
    auto vulkanWindow = std::make_shared<WindowVulkanLinux>(windowDesc, engineDesc.eventHandler);
    vulkanWindow->Create();
    engineDesc.graphics = std::make_shared<VulkanGraphics>(vulkanWindow->GetWindow(), vulkanWindow->GetConnection());
    engineDesc.window = vulkanWindow;
    return;
#endif
}

#if GL_SUPPORTED
    auto glWindow = std::make_shared<WindowGLLinux>(windowDesc, engineDesc.eventHandler);
    glWindow->Create();
    engineDesc.graphics = std::make_shared<GLGraphics>(glWindow->GetWindow(), glWindow->GetDisplay());
    engineDesc.window = glWindow;
    return;
#endif

#endif // PLATFORM_LINUX
    throw std::runtime_error("undefined platform");
}

static bool Run(bool useOpenGL) {
    WindowDesc windowDesc;
    try {
        windowDesc = GetWindowSettings();
    } catch(const std::exception& e) {
        LOG_ERROR_MESSAGE("Window settings initialization error: {}", e.what());
        return false;
    } catch(...) {
        LOG_ERROR_MESSAGE("Window settings initialization error: unhandled exception");
        return false;
    }

    EngineDesc engineDesc;
    try {
        engineDesc = GetEngineSettings();
    } catch(const std::exception& e) {
        LOG_ERROR_MESSAGE("Engine settings initialization error: {}", e.what());
        return false;
    } catch(...) {
        LOG_ERROR_MESSAGE("Engine settings initialization error: unhandled exception");
        return false;
    }

    try {
        CreatePlatform(useOpenGL, windowDesc, engineDesc);
    } catch(const std::exception& e) {
        LOG_ERROR_MESSAGE("Platform initialization error: {}", e.what());
        return false;
    } catch(...) {
        LOG_ERROR_MESSAGE("Platform initialization error: unhandled exception");
        return false;
    }

    auto& engine = Engine::Get();
    try {
        int validationLevel = -1;
        engineDesc.graphics->Create(validationLevel);
        engine.Create(std::move(engineDesc));
    } catch(const std::exception& e) {
        LOG_ERROR_MESSAGE("Engine initialization error: {}", e.what());
        return false;
    } catch(...) {
        LOG_ERROR_MESSAGE("Engine initialization error: unhandled exception");
        return false;
    }

    try {
        engine.Run();
        engine.Destroy();
    } catch(const std::exception& e) {
        LOG_ERROR_MESSAGE("Runtime error: {}", e.what());
        return false;
    } catch(...) {
        LOG_ERROR_MESSAGE("Runtime error: unhandled exception");
        return false;
    }

    return true;
}

int main() {
    return Run(false) ? EXIT_SUCCESS : EXIT_FAILURE;
}
