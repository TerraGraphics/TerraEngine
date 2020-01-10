#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
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

static void CreatePlatform(bool useOpenGL, WindowDesc windowDesc, EngineDesc& engineDesc, dg::DebugMessageCallbackType logMessageCallback) {
#ifdef PLATFORM_LINUX

if (!useOpenGL) {
#if VULKAN_SUPPORTED
    auto vulkanWindow = std::make_shared<WindowVulkanLinux>(windowDesc, engineDesc.eventHandler);
    vulkanWindow->Create();

    auto vulkanGraphics = std::make_shared<VulkanGraphics>(vulkanWindow->GetWindow(), vulkanWindow->GetConnection());
    Diligent::EngineVkCreateInfo& info = vulkanGraphics->GetCreateInfo();
    info.DynamicHeapSize = 8 << 21;
    info.DebugMessageCallback = logMessageCallback;

    engineDesc.window = vulkanWindow;
    engineDesc.graphics = vulkanGraphics;
    return;
#endif
}

#if GL_SUPPORTED
    auto glWindow = std::make_shared<WindowGLLinux>(windowDesc, engineDesc.eventHandler);
    glWindow->Create();

    auto glGraphics = std::make_shared<GLGraphics>(glWindow->GetWindow(), glWindow->GetDisplay());
    Diligent::EngineGLCreateInfo& info = glGraphics->GetCreateInfo();
    info.DebugMessageCallback = logMessageCallback;

    engineDesc.window = glWindow;
    engineDesc.graphics = glGraphics;
    return;
#endif

#endif // PLATFORM_LINUX
    throw std::runtime_error("undefined platform");
}

static bool Run(bool useOpenGL, spdlog::level::level_enum logLevel, bool logToFile) {
    spdlog::set_level(logLevel);
    if (logToFile) {
        auto file_logger = spdlog::basic_logger_mt("basic_logger", "rtge.log");
        spdlog::set_default_logger(file_logger);
    }

    auto logMessageCallback = [](dg::DebugMessageSeverity severity, const char* message, const char* function, const char* file, int line) {
        static spdlog::level::level_enum match[static_cast<size_t>(dg::DebugMessageSeverity::FatalError) + 1] = {
            spdlog::level::info, spdlog::level::warn, spdlog::level::err, spdlog::level::critical};
        spdlog::default_logger_raw()->log(spdlog::source_loc(file, line, function), match[static_cast<size_t>(severity)], message);
    };

    WindowDesc windowDesc;
    try {
        windowDesc = GetWindowSettings();
    } catch(const std::exception& e) {
        spdlog::error("Window settings initialization error: {}", e.what());
        return false;
    } catch(...) {
        spdlog::error("Window settings initialization error: unhandled exception");
        return false;
    }

    EngineDesc engineDesc;
    try {
        engineDesc = GetEngineSettings();
    } catch(const std::exception& e) {
        spdlog::error("Engine settings initialization error: {}", e.what());
        return false;
    } catch(...) {
        spdlog::error("Engine settings initialization error: unhandled exception");
        return false;
    }

    try {
        CreatePlatform(useOpenGL, windowDesc, engineDesc, logMessageCallback);
    } catch(const std::exception& e) {
        spdlog::error("Platform initialization error: {}", e.what());
        return false;
    } catch(...) {
        spdlog::error("Platform initialization error: unhandled exception");
        return false;
    }

    auto& engine = Engine::Get();
    try {
        int validationLevel = -1;
        engineDesc.graphics->Create(validationLevel);
        engine.Create(std::move(engineDesc));
    } catch(const std::exception& e) {
        spdlog::error("Engine initialization error: {}", e.what());
        return false;
    } catch(...) {
        spdlog::error("Engine initialization error: unhandled exception");
        return false;
    }

    try {
        engine.Run();
        engine.Destroy();
    } catch(const std::exception& e) {
        spdlog::error("Runtime error: {}", e.what());
        return false;
    } catch(...) {
        spdlog::error("Runtime error: unhandled exception");
        return false;
    }

    return true;
}

static bool IsCmdOptionExists(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

int main(int argc, char * argv[]) {
    bool useOpenGL = IsCmdOptionExists(argv, argv+argc, "-opengl");

    return Run(useOpenGL, spdlog::level::debug, false) ? EXIT_SUCCESS : EXIT_FAILURE;
}
