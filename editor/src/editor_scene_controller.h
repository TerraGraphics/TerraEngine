#pragma once

#include <memory>
#include <cstdint>

#include "core/common/ctor.h"


class Gui;
class RenderTarget;
class PreviewWindow;
class EditorSceneController : Fixed {
public:
    EditorSceneController();
    ~EditorSceneController();

public:
    void Create(uint32_t vsCameraVarId, uint32_t psCameraVarId, uint32_t gsCameraVarId, const std::shared_ptr<Gui>& gui);
    void Update(double deltaTime);
    void Draw();

private:
    void DockSpace();
    void PropertyWindow();
    void FooterWindow();

private:
    std::shared_ptr<Gui> m_gui = nullptr;
    std::unique_ptr<RenderTarget> m_renderTarget;
    std::unique_ptr<PreviewWindow> m_previewWindow;
};
