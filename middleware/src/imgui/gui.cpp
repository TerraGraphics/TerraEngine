#include "middleware/imgui/gui.h"

#include <string>
#include <cfloat>
#include <cstring>
#include <iterator>
#include <algorithm>
#include <filesystem>

#include "dg/math.h"
#include "dg/device.h"
#include "dg/context.h"
#include "imgui/imgui.h"
#include "platforms/platforms.h"
#include "middleware/imgui/font.h"
#include "core/common/exception.h"
#include "dg/shader_resource_binding.h"
#include "dg/shader_resource_variable.h"


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
    psIn.color = float4(pow(abs(vsIn.color.xyz), float3(2.2)), vsIn.color.w);
    psIn.uv = vsIn.uv;
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

namespace gui {

Gui::Gui(const DevicePtr& device, const ContextPtr& context, dg::TEXTURE_FORMAT backBufferFormat, dg::TEXTURE_FORMAT depthBufferFormat, const std::shared_ptr<RenderWindow>& window)
    : m_device(device)
    , m_context(context)
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

    // Docking options
    io.ConfigDockingNoSplit = false;
    io.ConfigDockingWithShift = false;
    io.ConfigDockingAlwaysTabBar = false;
    io.ConfigDockingTransparentPayload = false;

    // Platform
    io.BackendPlatformName = "terra";
    io.BackendRendererName = "terra engine";
    io.BackendPlatformUserData = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendLanguageUserData = nullptr;

    // Clipboard
    io.ClipboardUserData = reinterpret_cast<void*>(m_window.get());
    io.SetClipboardTextFn = [](void* window, const char* text) {
        reinterpret_cast<RenderWindow*>(window)->SetClipboard(text);
    };
    io.GetClipboardTextFn = [](void* window) -> const char* {
        return reinterpret_cast<RenderWindow*>(window)->GetClipboard();
    };


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

    CreateFonts();
    CreateGraphics();

    ImGui::GetStyle().ItemSpacing.x = 0;
}

void Gui::Update(double deltaTime, std::shared_ptr<DefaultWindowEventsHandler>& handler) {
    ImGuiIO& io = ImGui::GetIO();

    io.DeltaTime = static_cast<float>(deltaTime);

    uint32_t w, h;
    handler->GetWindowSize(w, h);
    io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));

    if (m_enableInput && handler->IsWindowFocused()) {
        io.MouseWheel += handler->GetScrollOffset();

        if (io.WantSetMousePos) {
            m_window->SetCursorPos(static_cast<int>(io.MousePos.x), static_cast<int>(io.MousePos.y));
        } else {
            float posX, posY;
            handler->GetCursorPosition(posX, posY);
            io.MousePos = ImVec2(posX, posY);
        }

        if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0) {
            ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
            if (io.MouseDrawCursor) {
                imgui_cursor = ImGuiMouseCursor_None;
            }
            switch (imgui_cursor)
            {
            case ImGuiMouseCursor_None:
                m_window->SetCursor(CursorType::Empty);
                break;
            case ImGuiMouseCursor_Arrow:
                m_window->SetCursor(CursorType::Arrow);
                break;
            case ImGuiMouseCursor_TextInput:
                m_window->SetCursor(CursorType::TextInput);
                break;
            case ImGuiMouseCursor_ResizeAll:
                m_window->SetCursor(CursorType::ResizeAll);
                break;
            case ImGuiMouseCursor_ResizeNS:
                m_window->SetCursor(CursorType::ResizeNS);
                break;
            case ImGuiMouseCursor_ResizeEW:
                m_window->SetCursor(CursorType::ResizeEW);
                break;
            case ImGuiMouseCursor_ResizeNESW:
                m_window->SetCursor(CursorType::ResizeNESW);
                break;
            case ImGuiMouseCursor_ResizeNWSE:
                m_window->SetCursor(CursorType::ResizeNWSE);
                break;
            case ImGuiMouseCursor_Hand:
                m_window->SetCursor(CursorType::Hand);
                break;
            case ImGuiMouseCursor_NotAllowed:
                m_window->SetCursor(CursorType::NotAllowed);
                break;
            default:
                m_window->SetCursor(CursorType::Arrow);
                break;
            }
        }

        handler->FillKeyboardKeysDown(io.KeysDown);
        io.KeyCtrl = handler->IsKeyDown(Key::Control);
        io.KeyShift = handler->IsKeyDown(Key::Shift);
        io.KeyAlt = handler->IsKeyDown(Key::Alt);
        io.KeySuper = handler->IsKeyDown(Key::Super);
        io.MouseDown[0] = handler->IsKeyStickyDown(Key::MouseLeft);
        io.MouseDown[1] = handler->IsKeyStickyDown(Key::MouseRight);
        io.MouseDown[2] = handler->IsKeyStickyDown(Key::MouseMiddle);

        for(const auto ch : handler->GetInput()) {
            io.AddInputCharacter(static_cast<unsigned int>(ch));
        }
    } else {
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);

        std::fill(std::begin(io.KeysDown), std::end(io.KeysDown), false);

        io.KeyCtrl = false;
        io.KeyShift = false;
        io.KeyAlt = false;
        io.KeySuper = false;
        io.MouseDown[0] = false;
        io.MouseDown[1] = false;
        io.MouseDown[2] = false;
    }
}

void Gui::StartFrame() {
    if (!m_ps) {
        CreateGraphics();
    }

    ImGui::NewFrame();
}

void Gui::RenderFrame() {
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
        void* vbRaw;
        void* ibRaw;
        m_context->MapBuffer(m_vb, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, vbRaw);
        m_context->MapBuffer(m_ib, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, ibRaw);

        ImDrawVert* vbIt = reinterpret_cast<ImDrawVert*>(vbRaw);
        ImDrawIdx* ibIt = reinterpret_cast<ImDrawIdx*>(ibRaw);
        for (int n = 0; n != drawData->CmdListsCount; ++n) {
            const ImDrawList* cmdList = drawData->CmdLists[n];
            memcpy(vbIt, cmdList->VtxBuffer.Data, static_cast<size_t>(cmdList->VtxBuffer.Size) * sizeof(ImDrawVert));
            memcpy(ibIt, cmdList->IdxBuffer.Data, static_cast<size_t>(cmdList->IdxBuffer.Size) * sizeof(ImDrawIdx));
            vbIt += cmdList->VtxBuffer.Size;
            ibIt += cmdList->IdxBuffer.Size;
        }

        m_context->UnmapBuffer(m_ib, dg::MAP_WRITE);
        m_context->UnmapBuffer(m_vb, dg::MAP_WRITE);
    }


    // Our visible imgui space lies from drawData->DisplayPos (top left) to drawData->DisplayPos+data_data->DisplaySize (bottom right).
    // DisplayPos is (0,0) for single viewport apps.
    {
        void* cameraDataRaw;
        m_context->MapBuffer(m_cameraCB, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, cameraDataRaw);
        dg::float4x4* cameraData = reinterpret_cast<dg::float4x4*>(cameraDataRaw);

        // TODO: check left and right matrix
        float L = drawData->DisplayPos.x;
        float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
        float T = drawData->DisplayPos.y;
        float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
        *cameraData = dg::float4x4 {
            2.0f / (R - L),                  0.0f,   0.0f,   0.0f,
            0.0f,                  2.0f / (T - B),   0.0f,   0.0f,
            0.0f,                            0.0f,   0.5f,   0.0f,
            (R + L) / (L - R),  (T + B) / (B - T),   0.5f,   1.0f};

        m_context->UnmapBuffer(m_cameraCB, dg::MAP_WRITE);
    }

    auto displayWidth = static_cast<uint32_t>(drawData->DisplaySize.x);
    auto displayHeight = static_cast<uint32_t>(drawData->DisplaySize.y);
    auto setupRenderState = [&]() {
        uint32_t offsets[] = {0};
        dg::IBuffer* vbBuffers[] = {m_vb};
        m_context->SetVertexBuffers(0, 1, vbBuffers, offsets, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, dg::SET_VERTEX_BUFFERS_FLAG_RESET);
        m_context->SetIndexBuffer(m_ib, 0, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        m_context->SetPipelineState(m_ps);
        m_context->CommitShaderResources(m_fontBinding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        const float blend_factor[4] = {0.f, 0.f, 0.f, 0.f};
        m_context->SetBlendFactors(blend_factor);

        dg::Viewport vp;
        vp.Width    = drawData->DisplaySize.x;
        vp.Height   = drawData->DisplaySize.y;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_context->SetViewports(1, &vp, displayWidth, displayHeight);
    };

    setupRenderState();

    // (Because we merged all buffers into a single one, we maintain our own offset into them)
    int globalIdxOffset = 0;
    int globalVtxOffset = 0;

    m_numberUsedBindings = 0;
    auto lastUsedTexture = m_fontTex;
    ImVec2 clip_off = drawData->DisplayPos;
    for (int n = 0; n < drawData->CmdListsCount; ++n) {
        const ImDrawList* cmdList = drawData->CmdLists[n];
        for (int cmdInd = 0; cmdInd < cmdList->CmdBuffer.Size; ++cmdInd) {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdInd];
            if (pcmd->UserCallback != NULL) {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == reinterpret_cast<ImDrawCallback>(-1) /* ImDrawCallback_ResetRenderState */) {
                    setupRenderState();
                    lastUsedTexture = m_fontTex;
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
                m_context->SetScissorRects(1, &r, displayWidth, displayHeight);

                auto* texture = reinterpret_cast<TextureViewRaw>(pcmd->TextureId);
                if (texture == nullptr) {
                    CreateFontsTexture();
                    lastUsedTexture = m_fontTex;
                    m_fontBinding->GetVariableByName(dg::SHADER_TYPE_PIXEL, "texBase")->Set(lastUsedTexture);
                    m_context->CommitShaderResources(m_fontBinding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                } else if (lastUsedTexture != texture) {
                    lastUsedTexture = texture;
                    ShaderResourceBindingPtr binding = m_fontBinding;

                    if (texture != m_fontTex) {
                        if (m_numberUsedBindings == m_bindings.size()) {
                            m_ps->CreateShaderResourceBinding(&binding, true);
                            m_bindings.push_back(binding);
                        } else {
                            binding = m_bindings[m_numberUsedBindings];
                        }
                        ++m_numberUsedBindings;
                        binding->GetVariableByName(dg::SHADER_TYPE_PIXEL, "texBase")->Set(lastUsedTexture);
                    }
                    m_context->CommitShaderResources(binding, dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                }
                dg::DrawIndexedAttribs DrawAttrs(pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? dg::VT_UINT16 : dg::VT_UINT32, dg::DRAW_FLAG_VERIFY_STATES);
                DrawAttrs.FirstIndexLocation = pcmd->IdxOffset + static_cast<uint32_t>(globalIdxOffset);
                DrawAttrs.BaseVertex         = pcmd->VtxOffset + static_cast<uint32_t>(globalVtxOffset);
                m_context->DrawIndexed(DrawAttrs);
            }
        }
        globalIdxOffset += cmdList->IdxBuffer.Size;
        globalVtxOffset += cmdList->VtxBuffer.Size;
    }
}

void Gui::CreateFonts() {
    ImGuiIO& io = ImGui::GetIO();

    // if (io.Fonts->AddFontDefault() == nullptr) {
    //     throw EngineError("failed to load a default font");
    // }

    {
        ImFontConfig config;
        config.OversampleH = 3;
        config.OversampleV = 1;
        config.PixelSnapH = false;
        const auto bFontPath = std::filesystem::current_path() / "assets" / "fonts" / "bfont.ttf";
        if (io.Fonts->AddFontFromFileTTF(bFontPath.c_str(), 18.0f, nullptr, nullptr) == nullptr) {
            throw EngineError("failed to load a font {}", bFontPath.c_str());
        }
    }

    {
        ImFontConfig config;
        config.MergeMode = true;
        config.PixelSnapH = true;
        static const ImWchar iconRanges[] = { startUsedRange, stopUsedRange, 0 };
        const auto faSolid900Path = std::filesystem::current_path() / "assets" / "fonts" / "fa-solid-900.ttf";
        if (io.Fonts->AddFontFromFileTTF(faSolid900Path.c_str(), 13.0f, &config, iconRanges) == nullptr) {
            throw EngineError("failed to load a font {}", faSolid900Path.c_str());
        }
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

    dg::BufferDesc cbDesc;
    cbDesc.uiSizeInBytes = sizeof(dg::float4x4);
    cbDesc.Usage = dg::USAGE_DYNAMIC;
    cbDesc.BindFlags = dg::BIND_UNIFORM_BUFFER;
    cbDesc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
    m_device->CreateBuffer(cbDesc, nullptr, &m_cameraCB);
    {
        void* cameraDataRaw;
        m_context->MapBuffer(m_cameraCB, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, cameraDataRaw);
        *reinterpret_cast<dg::float4x4*>(cameraDataRaw) = dg::float4x4();
        m_context->UnmapBuffer(m_cameraCB, dg::MAP_WRITE);
    }

    dg::LayoutElement layoutElems[] = {
        // vertex position
        dg::LayoutElement{0, 0, 2, dg::VT_FLOAT32, false},
        // texture coordinates
        dg::LayoutElement{1, 0, 2, dg::VT_FLOAT32, false},
        // color
        dg::LayoutElement{2, 0, 4, dg::VT_UINT8, true}
    };

    dg::ShaderResourceVariableDesc vars[] = {
        {dg::SHADER_TYPE_PIXEL, "texBase", dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC}
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

    dg::PipelineStateCreateInfo createInfo { desc, dg::PSO_CREATE_FLAG_NONE };
    m_device->CreatePipelineState(createInfo, &m_ps);

    m_ps->GetStaticVariableByName(dg::SHADER_TYPE_VERTEX, "Camera")->Set(m_cameraCB);
    m_ps->CreateShaderResourceBinding(&m_fontBinding, true);

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
    m_fontBinding->GetVariableByName(dg::SHADER_TYPE_PIXEL, "texBase")->Set(m_fontTex);
    io.Fonts->TexID = reinterpret_cast<ImTextureID>(m_fontTex.RawPtr());
}

void Gui::DestroyGraphics() {
    m_vb.Release();
    m_ib.Release();
    m_ps.Release();
    m_cameraCB.Release();
    m_fontBinding.Release();
    for(auto& binding: m_bindings) {
        binding.Release();
    }
    m_fontTex.Release();
}

}
