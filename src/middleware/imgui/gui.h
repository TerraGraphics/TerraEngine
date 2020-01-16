#pragma once

#include <memory>

#include "core/common/dg.h"
#include "core/common/ctor.h"


class RenderWindow;
class DefaultWindowEventsHandler;
class Gui : Fixed {
public:
    Gui() = delete;
    Gui(const DevicePtr& device, dg::TEXTURE_FORMAT backBufferFormat, dg::TEXTURE_FORMAT depthBufferFormat, const std::shared_ptr<RenderWindow>& window);
    ~Gui();

    void Create();
    void Update(double deltaTime, std::shared_ptr<DefaultWindowEventsHandler>& handler);
    void NewFrame();
    void EndFrame(dg::IDeviceContext* context);

private:
    void CreateGraphics();
    void CreateFontsTexture();
    void DestroyGraphics();

private:
    DevicePtr m_device;
    dg::TEXTURE_FORMAT m_backBufferFormat = dg::TEXTURE_FORMAT(0);
    dg::TEXTURE_FORMAT m_depthBufferFormat = dg::TEXTURE_FORMAT(0);
    std::shared_ptr<RenderWindow> m_window = nullptr;

    PipelineStatePtr m_ps;
    BufferPtr m_cameraCB;
    TextureViewPtr m_fontTex;
    ShaderResourceBindingPtr m_binding;
    BufferPtr m_vb;
    BufferPtr m_ib;
    uint32_t m_vbSize = 1024;
    uint32_t m_ibSize = 2048;
};
