#include <DiligentCore/Primitives/interface/Errors.h>

#include "editor.h"
#include "core/engine.h"
#include "platforms/platforms_supported.h"


static void CreatePlatform(EngineDesc& desc) {
    // desc.eventHandler = std::make_shared<DebugWindowEventsHandler>();
    desc.eventHandler = std::make_shared<DefaultWindowEventsHandler>();

#ifdef PLATFORM_LINUX

#if VULKAN_SUPPORTED
    auto vulkanWindow = std::make_shared<WindowVulkanLinux>(desc.eventHandler);
    vulkanWindow->Create(0, 0, 800, 600, 0, "terra");
    desc.graphics = std::make_shared<VulkanGraphics>(vulkanWindow->GetWindow(), vulkanWindow->GetConnection());
    desc.window = vulkanWindow;
    return;
#endif

#if GL_SUPPORTED
    auto glWindow = std::make_shared<WindowGLLinux>(desc.eventHandler);
    glWindow->Create(0, 0, 800, 600, "terra");
    desc.graphics = std::make_shared<GLGraphics>(glWindow->GetWindow(), glWindow->GetDisplay());
    desc.window = glWindow;
    return;
#endif

#endif // PLATFORM_LINUX
    throw std::runtime_error("undefined platform");
}

static bool Run() {
    int validationLevel = -1;

    try {
        EngineDesc desc;
        desc.isVSync = true;

        try {

            CreatePlatform(desc);
            desc.graphics->Create(validationLevel);
        } catch(const std::exception& e) {
            LOG_ERROR_MESSAGE("Platform initialization error: {}", e.what());
            return false;
        }

        auto& engine = Engine::Get();
        try {
            desc.application = std::make_unique<Editor>();
            engine.Create(std::move(desc));
        } catch(const std::exception& e) {
            LOG_ERROR_MESSAGE("Engine initialization error: {}", e.what());
            return false;
        }

        engine.Run();
        engine.Destroy();
    } catch(const std::exception& e) {
        LOG_ERROR_MESSAGE("Runtime exception: {}", e.what());
        return false;
    } catch(...) {
        LOG_ERROR_MESSAGE("Unhandled exception");
        return false;
    }

    return true;
}

int main() {
    return Run() ? EXIT_SUCCESS : EXIT_FAILURE;
}
