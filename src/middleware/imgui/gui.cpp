#include "middleware/imgui/gui.h"

#include <imgui.h>

#include <DiligentCore/Common/interface/BasicMath.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/MapHelper.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>

#include "platforms/platforms.h"
#include "core/common/exception.h"

static const char* vertexShaderSource = R"(
cbuffer Camera {
    float4x4 matProj;
}

struct VSInput {
    float2 position : ATTRIB0;
    float2 uv       : ATTRIB1;
    float4 color    : ATTRIB2;
};

struct PSInput {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 uv       : TEXCOORD;
};

void main(in VSInput vsIn, out PSInput psIn) {
    psIn.position = mul(matProj, float4(vsIn.position.xy, 0.0, 1.0));
    psIn.color = vsIn.color;
    psIn.uv  = vsIn.uv;
}
)";

static const char* pixelShaderSource = R"(
struct PSInput {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
    float2 uv       : TEXCOORD;
};

Texture2D    texBase;
SamplerState texBaseSampler;

float4 main(in PSInput psIn) : SV_TARGET {
    return psIn.color * texBase.Sample(texBaseSampler, psIn.uv);
}
)";


Gui::Gui(const DevicePtr& device, dg::TEXTURE_FORMAT backBufferFormat, dg::TEXTURE_FORMAT depthBufferFormat, const std::shared_ptr<RenderWindow>& window)
    : m_device(device)
    , m_backBufferFormat(backBufferFormat)
    , m_depthBufferFormat(depthBufferFormat)
    , m_window(window) {

}

Gui::~Gui() {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui::DestroyContext();
    }
    DestroyGraphics();
    m_device.Release();
    m_window.reset();
}

void Gui::Create() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    if (ImGui::GetCurrentContext() == nullptr) {
        throw EngineError("failed to create ImGui context");
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableSetMousePos | ImGuiConfigFlags_DockingEnable;
    io.BackendFlags = ImGuiBackendFlags_HasMouseCursors | ImGuiBackendFlags_HasSetMousePos | ImGuiBackendFlags_RendererHasVtxOffset;
    io.BackendPlatformName = "terra";
    io.BackendRendererName = "terra engine";
    io.MouseDrawCursor = false;
    // TODO: add clipboard
    // io.SetClipboardTextFn = Window::SetClipboardText;
    // io.GetClipboardTextFn = Window::GetClipboardText;
    // io.ClipboardUserData = nullptr;

    // TODO: check flags
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    io.KeyMap[ImGuiKey_Tab] = static_cast<int>(Key::Tab);
    io.KeyMap[ImGuiKey_LeftArrow] = static_cast<int>(Key::ArrowLeft);
    io.KeyMap[ImGuiKey_RightArrow] = static_cast<int>(Key::ArrowRight);
    io.KeyMap[ImGuiKey_UpArrow] = static_cast<int>(Key::ArrowUp);
    io.KeyMap[ImGuiKey_DownArrow] = static_cast<int>(Key::ArrowDown);
    io.KeyMap[ImGuiKey_PageUp] = static_cast<int>(Key::PageUp);
    io.KeyMap[ImGuiKey_PageDown] = static_cast<int>(Key::PageDown);
    io.KeyMap[ImGuiKey_Home] = static_cast<int>(Key::Home);
    io.KeyMap[ImGuiKey_End] = static_cast<int>(Key::End);
    io.KeyMap[ImGuiKey_Insert] = static_cast<int>(Key::Insert);
    io.KeyMap[ImGuiKey_Delete] = static_cast<int>(Key::Delete);
    io.KeyMap[ImGuiKey_Backspace] = static_cast<int>(Key::Backspace);
    io.KeyMap[ImGuiKey_Space] = static_cast<int>(Key::Space);
    io.KeyMap[ImGuiKey_Enter] = static_cast<int>(Key::Enter);
    io.KeyMap[ImGuiKey_Escape] = static_cast<int>(Key::Escape);
    io.KeyMap[ImGuiKey_KeyPadEnter] = static_cast<int>(Key::KeyPadEnter);
    io.KeyMap[ImGuiKey_A] = static_cast<int>(Key::A);
    io.KeyMap[ImGuiKey_C] = static_cast<int>(Key::C);
    io.KeyMap[ImGuiKey_V] = static_cast<int>(Key::V);
    io.KeyMap[ImGuiKey_X] = static_cast<int>(Key::X);
    io.KeyMap[ImGuiKey_Y] = static_cast<int>(Key::Y);
    io.KeyMap[ImGuiKey_Z] = static_cast<int>(Key::Z);

    ImGui::StyleColorsDark();

    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    mainViewport->PlatformHandle = m_window->GetNativeWindowHandler();

    CreateGraphics();
}

void Gui::Update(double deltaTime, std::shared_ptr<DefaultWindowEventsHandler>& handler) {
    ImGuiIO& io = ImGui::GetIO();

    io.DeltaTime = static_cast<float>(deltaTime);

    uint32_t w, h;
    handler->GetWindowSize(w, h);
    io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
}

void Gui::NewFrame() {
    if (!m_ps) {
        CreateGraphics();
    }

    ImGui::NewFrame();
}

void Gui::EndFrame(dg::IDeviceContext* context) {
    ImGui::Render();
    ImDrawData* drawData = ImGui::GetDrawData();
    if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f) {
        return;
    }

    if (!m_vb || static_cast<int>(m_vbSize) < drawData->TotalVtxCount) {
        m_vb.Release();
        while (static_cast<int>(m_vbSize) < drawData->TotalVtxCount) {
            m_vbSize *= 2;
        }

        dg::BufferDesc desc;
        desc.Name           = "vb::imgui";
        desc.BindFlags      = dg::BIND_VERTEX_BUFFER;
        desc.uiSizeInBytes  = m_vbSize * sizeof(ImDrawVert);
        desc.Usage          = dg::USAGE_DYNAMIC;
        desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
        m_device->CreateBuffer(desc, nullptr, &m_vb);
    }

    if (!m_ib || static_cast<int>(m_ibSize) < drawData->TotalIdxCount) {
        m_ib.Release();
        while (static_cast<int>(m_ibSize) < drawData->TotalIdxCount) {
            m_ibSize *= 2;
        }

        dg::BufferDesc desc;
        desc.Name           = "ib::imgui";
        desc.BindFlags      = dg::BIND_INDEX_BUFFER;
        desc.uiSizeInBytes  = m_ibSize * sizeof(ImDrawIdx);
        desc.Usage          = dg::USAGE_DYNAMIC;
        desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
        m_device->CreateBuffer(desc, nullptr, &m_ib);
    }

    {
        dg::MapHelper<ImDrawVert> vbData(context, m_vb, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);
        dg::MapHelper<ImDrawIdx>  ibData(context, m_ib, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);

        ImDrawVert* vbIt = vbData;
        ImDrawIdx* ibIt = ibData;
        for (int n = 0; n < drawData->CmdListsCount; ++n) {
            const ImDrawList* cmdList = drawData->CmdLists[n];
            memcpy(vbIt, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
            memcpy(ibIt, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
            vbIt += cmdList->VtxBuffer.Size;
            ibIt += cmdList->IdxBuffer.Size;
        }
    }

    // Our visible imgui space lies from drawData->DisplayPos (top left) to drawData->DisplayPos+data_data->DisplaySize (bottom right).
    // DisplayPos is (0,0) for single viewport apps.
    {
        dg::MapHelper<dg::float4x4> CBData(context, m_cameraCB, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD);

        // TODO: check left and right matrix
        float L = drawData->DisplayPos.x;
        float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
        float T = drawData->DisplayPos.y;
        float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
        *CBData = dg::float4x4 {
            2.0f / (R - L),                  0.0f,   0.0f,   0.0f,
            0.0f,                  2.0f / (T - B),   0.0f,   0.0f,
            0.0f,                            0.0f,   0.5f,   0.0f,
            (R + L) / (L - R),  (T + B) / (B - T),   0.5f,   1.0f};
    }

    auto displayWidth = static_cast<uint32_t>(drawData->DisplaySize.x);
    auto displayHeight = static_cast<uint32_t>(drawData->DisplaySize.y);
    auto setupRenderState = [&]() {
        uint32_t offsets[] = {0};
        dg::IBuffer* vbBuffers[] = {m_vb};
        context->SetVertexBuffers(0, 1, vbBuffers, offsets, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::SET_VERTEX_BUFFERS_FLAG_RESET);
        context->SetIndexBuffer(m_ib, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        context->SetPipelineState(m_ps);
        context->CommitShaderResources(m_binding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float blend_factor[4] = {0.f, 0.f, 0.f, 0.f};
        context->SetBlendFactors(blend_factor);

        dg::Viewport vp;
        vp.Width    = drawData->DisplaySize.x;
        vp.Height   = drawData->DisplaySize.y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        context->SetViewports(1, &vp, displayWidth, displayHeight);
    };

    setupRenderState();

    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int globalIdxOffset = 0;
    int globalVtxOffset = 0;

    ImVec2 clip_off = drawData->DisplayPos;
    for (int n = 0; n < drawData->CmdListsCount; ++n) {
        const ImDrawList* cmdList = drawData->CmdLists[n];
        for (int cmdInd = 0; cmdInd < cmdList->CmdBuffer.Size; ++cmdInd) {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdInd];
            if (pcmd->UserCallback != NULL) {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState) {
                    setupRenderState();
                } else {
                    pcmd->UserCallback(cmdList, pcmd);
                }
            } else {
                // Apply scissor/clipping rectangle
                const dg::Rect r = {
                    static_cast<int32_t>(pcmd->ClipRect.x - clip_off.x),
                    static_cast<int32_t>(pcmd->ClipRect.y - clip_off.y),
                    static_cast<int32_t>(pcmd->ClipRect.z - clip_off.x),
                    static_cast<int32_t>(pcmd->ClipRect.w - clip_off.y)};
                context->SetScissorRects(1, &r, displayWidth, displayHeight);

                // Bind texture, Draw
                auto* textureSrv = reinterpret_cast<dg::ITextureView*>(pcmd->TextureId);
                VERIFY_EXPR(textureSrv == m_fontTex);
                (void)textureSrv;
                //ctx->PSSetShaderResources(0, 1, &textureSrv);
                dg::DrawIndexedAttribs DrawAttrs(pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? dg::VT_UINT16 : dg::VT_UINT32, dg::DRAW_FLAG_VERIFY_STATES);
                DrawAttrs.FirstIndexLocation = pcmd->IdxOffset + globalIdxOffset;
                DrawAttrs.BaseVertex         = pcmd->VtxOffset + globalVtxOffset;
                context->DrawIndexed(DrawAttrs);
            }
        }
        globalIdxOffset += cmdList->IdxBuffer.Size;
        globalVtxOffset += cmdList->VtxBuffer.Size;
    }
}

void Gui::CreateGraphics() {
    DestroyGraphics();

    dg::ShaderCreateInfo shaderCI;
    shaderCI.EntryPoint = "main";
    shaderCI.UseCombinedTextureSamplers = true;
    shaderCI.CombinedSamplerSuffix = "Sampler";
    shaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;

    ShaderPtr vsShader;
    {
        shaderCI.Source = vertexShaderSource;
        shaderCI.Desc.ShaderType = dg::SHADER_TYPE_VERTEX;
        shaderCI.Desc.Name = "vs::imgui";
        m_device->CreateShader(shaderCI, &vsShader);
    }

    ShaderPtr psShader;
    {
        shaderCI.Source = pixelShaderSource;
        shaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
        shaderCI.Desc.Name = "ps::imgui";
        m_device->CreateShader(shaderCI, &psShader);
    }

    dg::LayoutElement layoutElems[] = {
        // vertex position
        dg::LayoutElement{0, 0, 2, dg::VT_FLOAT32, dg::False},
        // texture coordinates
        dg::LayoutElement{1, 0, 2, dg::VT_FLOAT32, dg::False},
        // color
        dg::LayoutElement{1, 0, 4, dg::VT_UINT8, dg::True}
    };

    dg::ShaderResourceVariableDesc vars[] = {
        {dg::SHADER_TYPE_PIXEL, "texBase", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE}
    };

    dg::SamplerDesc samplerDesc;
    samplerDesc.AddressU = dg::TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = dg::TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = dg::TEXTURE_ADDRESS_WRAP;

    dg::StaticSamplerDesc samplers[] = {
        {dg::SHADER_TYPE_PIXEL, "texBase", samplerDesc}
    };

    dg::PipelineStateDesc desc;
    desc.Name = "imgui";
    desc.IsComputePipeline = false;

    auto& gp = desc.GraphicsPipeline;
    gp.NumRenderTargets = 1;
    gp.PrimitiveTopology = dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    gp.DepthStencilDesc.DepthEnable = false;
    gp.pVS = vsShader;
    gp.pPS = psShader;
    gp.InputLayout.LayoutElements = layoutElems;
    gp.InputLayout.NumElements = _countof(layoutElems);
    gp.RasterizerDesc.CullMode = dg::CULL_MODE_NONE;
    gp.RasterizerDesc.ScissorEnable = false;
    gp.RTVFormats[0] = m_backBufferFormat;
    gp.DSVFormat = m_depthBufferFormat;

    auto& rt = gp.BlendDesc.RenderTargets[0];
    rt.BlendEnable = true;
    rt.SrcBlend = dg::BLEND_FACTOR_SRC_ALPHA;
    rt.DestBlend = dg::BLEND_FACTOR_INV_SRC_ALPHA;
    rt.BlendOp = dg::BLEND_OPERATION_ADD;
    rt.SrcBlendAlpha = dg::BLEND_FACTOR_INV_SRC_ALPHA;
    rt.DestBlendAlpha = dg::BLEND_FACTOR_ZERO;
    rt.BlendOpAlpha = dg::BLEND_OPERATION_ADD;
    rt.RenderTargetWriteMask = dg::COLOR_MASK_ALL;

    auto& rl = desc.ResourceLayout;
    rl.DefaultVariableType = dg::SHADER_RESOURCE_VARIABLE_TYPE_STATIC;
    rl.Variables = vars;
    rl.NumVariables = _countof(vars);
    rl.StaticSamplers = samplers;
    rl.NumStaticSamplers = _countof(samplers);

    m_device->CreatePipelineState(desc, &m_ps);

    dg::BufferDesc cbDesc;
    cbDesc.uiSizeInBytes = sizeof(dg::float4x4);
    cbDesc.Usage = dg::USAGE_DYNAMIC;
    cbDesc.BindFlags = dg::BIND_UNIFORM_BUFFER;
    cbDesc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
    m_device->CreateBuffer(cbDesc, nullptr, &m_cameraCB);

    m_ps->GetStaticVariableByName(dg::SHADER_TYPE_VERTEX, "Camera")->Set(m_cameraCB);

    CreateFontsTexture();
}

void Gui::CreateFontsTexture() {
    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels = nullptr;
    int width = 0;
    int height = 0;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    dg::TextureDesc fontTexDesc;
    fontTexDesc.Name = "tex::imgui::font";
    fontTexDesc.Type = dg::RESOURCE_DIM_TEX_2D;
    fontTexDesc.Width = static_cast<uint32_t>(width);
    fontTexDesc.Height = static_cast<uint32_t>(height);
    fontTexDesc.Format = dg::TEX_FORMAT_RGBA8_UNORM;
    fontTexDesc.BindFlags = dg::BIND_SHADER_RESOURCE;
    fontTexDesc.Usage = dg::USAGE_STATIC;

    dg::TextureSubResData subResources[] = {
        {pixels, fontTexDesc.Width * 4}
    };
    dg::TextureData data(subResources, _countof(subResources));

    TexturePtr fontTex;
    m_device->CreateTexture(fontTexDesc, &data, &fontTex);
    m_fontTex = fontTex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);

    m_binding.Release();
    m_ps->CreateShaderResourceBinding(&m_binding, true);
    m_binding->GetVariableByName(dg::SHADER_TYPE_PIXEL, "texBase")->Set(m_fontTex);
    io.Fonts->TexID = (ImTextureID)m_fontTex;
}

void Gui::DestroyGraphics() {
    m_vb.Release();
    m_ib.Release();
    m_ps.Release();
    m_cameraCB.Release();
    m_binding.Release();
    m_fontTex.Release();
}
