#include "editor/editor_scene.h"

#include <DiligentTools/TextureLoader/interface/TextureUtilities.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>

#include "core/engine.h"
#include "core/scene/scene.h"
#include "middleware/gizmo/gizmo_3d.h"
#include "core/material/material_builder.h"
#include "middleware/generator/generator.h"
#include "middleware/std_material/std_material.h"


EditorScene::EditorScene() {

}

EditorScene::~EditorScene() {

}

void EditorScene::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_scene = std::make_shared<Scene>(m_device, engine.GetImmediateContext());

    CreateTextures();
    CreateMaterials();
    GenerateCube();
    GenerateGizmo();
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
    const auto BASE_COLOR_MATERIAL = materialBuilder->GetShaderMask("BASE_COLOR_MATERIAL");
    const auto BASE_COLOR_TEXTURE = materialBuilder->GetShaderMask("BASE_COLOR_TEXTURE");
    const auto ALPHA_TEST = materialBuilder->GetShaderMask("ALPHA_TEST");
    const auto AMBIENT_DIFFUSE_PHONG = materialBuilder->GetShaderMask("AMBIENT_DIFFUSE_PHONG");
    const auto& vDecl = VertexPNC::GetDecl();

    m_matTexNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE, vDecl).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::noLight");

    m_matTexDiscardNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE | ALPHA_TEST, vDecl).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::discard::noLight");

    m_matTexPhong = materialBuilder->Create(BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG, vDecl).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::phong");

    m_matClrNoLight = materialBuilder->Create(BASE_COLOR_MATERIAL, vDecl).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::noLight");

    m_matClrPhong = materialBuilder->Create(BASE_COLOR_MATERIAL | AMBIENT_DIFFUSE_PHONG, vDecl).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::phong");
}

void EditorScene::GenerateCube() {
    // ConeShape shape({30, 30}, Axis::Y);
    SphereShape shape({30, 30}, Axis::Y);
    // PlaneShape shape(UInt2(1, 1), {Axis::X, Axis::Z});
    // shape.SetTexScale({1, 1});
    auto model = ShapeBuilder(m_device).Join({&shape}, "Model");

    auto modelNode = std::make_shared<StdMaterial>(m_matTexPhong, model);
    modelNode->SetBaseTexture(m_TextureCube);

    auto matModel = dg::float4x4::Scale(1, 1, 1);
    m_scene->NewChild(modelNode, matModel);
}

void EditorScene::GenerateGizmo() {
    m_gizmo = std::make_unique<Gizmo3D>();
    m_scene->AddChild(m_gizmo->Create(m_device, Engine::Get().GetMaterialBuilder()));
}
