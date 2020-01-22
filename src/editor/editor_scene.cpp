#include "editor/editor_scene.h"

#include <DiligentTools/TextureLoader/interface/TextureUtilities.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>

#include "core/engine.h"
#include "core/scene/scene.h"
#include "core/scene/material_node.h"
#include "core/material/material_builder.h"
#include "middleware/generator/mesh_generator.h"


static dg::LayoutElement layoutElems[] = {
    // vertex position
    dg::LayoutElement{0, 0, 3, dg::VT_FLOAT32, dg::False},
    // normal
    dg::LayoutElement{1, 0, 3, dg::VT_FLOAT32, dg::False},
    // texture coordinates
    dg::LayoutElement{2, 0, 2, dg::VT_FLOAT32, dg::False},

    // Per-instance data - second buffer slot
    // WorldRow0
    dg::LayoutElement{3, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow1
    dg::LayoutElement{4, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow2
    dg::LayoutElement{5, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow3
    dg::LayoutElement{6, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow0
    dg::LayoutElement{7, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow1
    dg::LayoutElement{8, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow2
    dg::LayoutElement{9, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
};

static dg::InputLayoutDesc layoutDesc(layoutElems, _countof(layoutElems));


void EditorScene::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_scene = std::make_shared<Scene>(m_device, engine.GetImmediateContext());

    CreateTextures();
    CreateMaterials();
    GenerateCube();
}

void EditorScene::Update(double deltaTime) {
    m_scene->Update();
}

void EditorScene::Draw() {
    m_scene->Draw();
}

void EditorScene::CreateTextures() {
    dg::TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;

    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/ground.jpg", loadInfo, m_device, &Tex);
        m_TextureCube = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
}

void EditorScene::CreateMaterials() {
    auto materialBuilder = Engine::Get().GetMaterialBuilder();
    const auto BASE_COLOR_TEXTURE = materialBuilder->GetShaderMask("BASE_COLOR_TEXTURE");
    const auto ALPHA_TEST = materialBuilder->GetShaderMask("ALPHA_TEST");
    const auto AMBIENT_DIFFUSE_PHONG = materialBuilder->GetShaderMask("AMBIENT_DIFFUSE_PHONG");
    const auto GRASS = materialBuilder->GetShaderMask("GRASS");

    m_matTexNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::noLight");

    m_matTexDiscardNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE | ALPHA_TEST, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::discard::noLight");

    m_matTexPhong = materialBuilder->Create(BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::phong");

    m_matClrNoLight = materialBuilder->Create(0, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::noLight");

    m_matClrPhong = materialBuilder->Create(AMBIENT_DIFFUSE_PHONG, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::phong");
}

void EditorScene::GenerateCube() {
    auto cube = MeshGenerator::CreateSolidCube(m_device);

    auto cubeNode = std::make_shared<MaterialNode>(m_matTexPhong, cube);
    cubeNode->SetPixelShaderVar("texBase", m_TextureCube);

    auto matModel = dg::float4x4::Scale(1, 1, 1);
    m_scene->NewChild(cubeNode, matModel);
}
