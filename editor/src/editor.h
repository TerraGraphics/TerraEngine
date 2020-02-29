#pragma once

#include <memory>

#include "core/dg/dg.h"
#include "core/engine.h"
#include "core/common/counter.h"


namespace Diligent {
    struct DisplayModeAttribs;
}

class Gui;
struct ImFont;
class EditorSceneController;
class GeneralSceneController;
class Editor final : public Application {
public:
    Editor();
    ~Editor() override;

    void Create() override;
    void Update(double deltaTime) override;
    void Draw() override;
    void Destroy() override;

    virtual void SetFullscreenMode(const dg::DisplayModeAttribs& DisplayMode);
    virtual void SetWindowedMode();

private:
    DevicePtr m_device;
    ContextPtr m_context;
    SwapChainPtr m_swapChain;

    bool m_bFullScreenMode = false;

    CyclicalCounter<double, 120> m_performanceCounter = CyclicalCounter<double, 120>(1./100.);
    std::shared_ptr<Gui> m_gui = nullptr;
    std::shared_ptr<EditorSceneController> m_editorSceneController = nullptr;
    std::shared_ptr<GeneralSceneController> m_generalSceneController = nullptr;

    ImFont* m_fontDefault = nullptr;
    ImFont* m_fontMono = nullptr;
};
