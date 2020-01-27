#pragma once

#include <memory>
#include <vector>

#include "core/common/dg.h"
#include "core/common/ctor.h"


class RenderWindow;
class DefaultWindowEventsHandler;
class Gui : Fixed {
public:
    Gui() = delete;
    Gui(const DevicePtr& device, const ContextPtr& context, dg::TEXTURE_FORMAT backBufferFormat, dg::TEXTURE_FORMAT depthBufferFormat, const std::shared_ptr<RenderWindow>& window);
    ~Gui();

    void Create();
    void Update(double deltaTime, std::shared_ptr<DefaultWindowEventsHandler>& handler);
    void StartFrame();
    void RenderFrame();

private:
    void CreateGraphics();
    void CreateFontsTexture();
    void DestroyGraphics();

private:
    DevicePtr m_device;
    ContextPtr m_context;
    dg::TEXTURE_FORMAT m_backBufferFormat = dg::TEXTURE_FORMAT(0);
    dg::TEXTURE_FORMAT m_depthBufferFormat = dg::TEXTURE_FORMAT(0);
    std::shared_ptr<RenderWindow> m_window = nullptr;

    PipelineStatePtr m_ps;
    BufferPtr m_cameraCB;
    TextureViewPtr m_fontTex;
    ShaderResourceBindingPtr m_fontBinding;
    std::vector<ShaderResourceBindingPtr> m_bindings;
    uint32_t m_numberUsedBindings = 0;
    BufferPtr m_vb;
    BufferPtr m_ib;
    uint32_t m_vbSize = 1024;
    uint32_t m_ibSize = 2048;

    bool m_enableInput = true;
};
