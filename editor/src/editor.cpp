#include "editor.h"

#include <string>
#include <ctype.h>
#include <filesystem>
#include <functional>

#include "core/path/path.h"
#include "core/dg/device.h" // IWYU pragma: keep
#include "core/dg/context.h" // IWYU pragma: keep
#include "core/str/format.h"
#include "core/common/timer.h"
#include "platforms/platforms.h"
#include "middleware/imgui/gui.h"
#include "editor/editor_scene_controller.h"
#include "core/material/material_builder.h"
#include "editor/general_scene_controller.h"
#include "core/material/material_builder_desc.h"


Editor::Editor() {

}

Editor::~Editor() {

}

void Editor::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_swapChain = engine.GetSwapChain();
    m_context = engine.GetContext();

    auto fileManager = engine.GetFileManager();
    // fileManager->AddRootAlias("$shader", fileManager->CurrentPath() / "materials");

    MaterialBuilderDesc materialDesc;
    materialDesc.samplerSuffix = "Sampler";
    materialDesc.shadersDir = fileManager->CurrentPath() / "materials" / "std";
    materialDesc.shadersSchemaPath = fileManager->CurrentPath() / "materials" / "schema" / "msh.schema.json";
    materialDesc.shaderFilesExtension = ".msh";
    materialDesc.cbufferNameGenerator = [](const std::string& value) -> std::string {
        auto res = value;
        res[0] = static_cast<char>(std::toupper(value[0]));
        return res;
    };

    auto& materialBuilder = engine.GetMaterialBuilder();
    materialBuilder->Load(materialDesc);

    // engine.GetWindow()->SetCursor(CursorType::Disabled);
    engine.GetWindow()->SetCursor(CursorType::Arrow);

    m_gui = std::make_shared<gui::Gui>(m_device, m_context, m_swapChain->GetDesc().ColorBufferFormat, m_swapChain->GetDesc().DepthBufferFormat, engine.GetWindow());
    m_gui->Create();

    m_editorSceneController = std::make_shared<EditorSceneController>();
    m_editorSceneController->Create(m_gui);
    m_generalSceneController = std::make_shared<GeneralSceneController>();
    m_generalSceneController->Create();
}

void Editor::Update(double deltaTime) {
    if (!m_device) {
        return;
    }

    auto& engine = Engine::Get();

    auto handler = engine.GetEventHandler();
    if (handler->IsKeyReleasedFirstTime(Key::Escape)) {
        handler->OnWindowDestroy();
    }

    m_gui->Update(deltaTime, handler);

    Timer timer;
    timer.Start();
    m_editorSceneController->Update(deltaTime);
    // m_generalSceneController->Update(deltaTime);
    m_performanceCounter.Add(timer.TimePoint());

    if (m_performanceCounter.Index() % 30 == 0) {
        engine.GetWindow()->SetTitle(fmt::format("fps = {:.1f}, update = {:.1f}", engine.GetFps(), m_performanceCounter.Avg() * 1000.).c_str());
    }
}

void Editor::Draw() {
    m_editorSceneController->Draw();
    // m_generalSceneController->Draw();
}

void Editor::Destroy() {
    m_editorSceneController.reset();
    m_generalSceneController.reset();
    m_gui.reset();
}

void Editor::SetFullscreenMode(const dg::DisplayModeAttribs& DisplayMode) {
    m_bFullScreenMode = true;
    m_swapChain->SetFullscreenMode(DisplayMode);
}

void Editor::SetWindowedMode() {
    m_bFullScreenMode = false;
    m_swapChain->SetWindowedMode();
}
