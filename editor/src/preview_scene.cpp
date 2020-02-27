#include "preview_scene.h"

#include "core/engine.h"
#include "core/scene/scene.h"
#include "core/scene/vertexes.h"
#include "core/dg/device_context.h"
#include "core/dg/texture_utilities.h"
#include "core/material/material_builder.h"
#include "middleware/generator/generator.h"
#include "middleware/std_material/std_material.h"


static const auto& additionalVDecl = VertexDecl({
        ItemDecl<dg::float4>("WorldRow0", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float4>("WorldRow1", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float4>("WorldRow2", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float4>("WorldRow3", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float3>("NormalRow0", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float3>("NormalRow1", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
        ItemDecl<dg::float3>("NormalRow2", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
        ItemDecl<math::Color4>("IdColor", 1, dg::LayoutElement::FREQUENCY_PER_INSTANCE),
    });

PreviewScene::PreviewScene() {

}

PreviewScene::~PreviewScene() {

}

void PreviewScene::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_scene = std::make_shared<Scene>(m_device, engine.GetImmediateContext(), true);

    CreateTextures();
    CreateMaterials();
    GenerateMeshes();
}

void PreviewScene::AddChild(const std::shared_ptr<TransformNode>& node) {
    m_scene->AddChild(node);
}

std::shared_ptr<TransformNode> PreviewScene::Update(double /* deltaTime */, uint32_t findId) {
    return m_scene->Update(findId);
}

void PreviewScene::Draw() {
    m_scene->Draw();
}

void PreviewScene::CreateTextures() {
    dg::TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;

    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/ground.jpg", loadInfo, m_device, &Tex);
        m_TextureCube = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
}

void PreviewScene::CreateMaterials() {
    auto materialBuilder = Engine::Get().GetMaterialBuilder();
    const auto BASE_COLOR_MATERIAL = materialBuilder->GetShaderMask("BASE_COLOR_MATERIAL");
    const auto BASE_COLOR_TEXTURE = materialBuilder->GetShaderMask("BASE_COLOR_TEXTURE");
    const auto ALPHA_TEST = materialBuilder->GetShaderMask("ALPHA_TEST");
    const auto AMBIENT_DIFFUSE_PHONG = materialBuilder->GetShaderMask("AMBIENT_DIFFUSE_PHONG");
    const auto COLOR_PICKER = materialBuilder->GetShaderMask("COLOR_PICKER");
    const auto& vDecl = VertexPNC::GetDecl();

    m_matTexNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE | COLOR_PICKER, vDecl, additionalVDecl).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::noLight");

    m_matTexDiscardNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE | ALPHA_TEST | COLOR_PICKER, vDecl, additionalVDecl).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::discard::noLight");

    m_matTexPhong = materialBuilder->Create(BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG | COLOR_PICKER, vDecl, additionalVDecl).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::phong");

    m_matClrNoLight = materialBuilder->Create(BASE_COLOR_MATERIAL | COLOR_PICKER, vDecl, additionalVDecl).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::noLight");

    m_matClrPhong = materialBuilder->Create(BASE_COLOR_MATERIAL | AMBIENT_DIFFUSE_PHONG | COLOR_PICKER, vDecl, additionalVDecl).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::phong");
}

void PreviewScene::GenerateMeshes() {
    // ConeShape shape({30, 30}, math::Axis::Y);
    ConeShape shape1({30, 30}, math::Axis::Y);
    SphereShape shape2({30, 30}, math::Axis::Y);
    // PlaneShape shape(UInt2(1, 1), {Axis::X, Axis::Z});
    // shape.SetTexScale({1, 1});
    auto model1 = ShapeBuilder(m_device).Join({&shape1}, "Model1");
    auto model2 = ShapeBuilder(m_device).Join({&shape2}, "Model2");

    auto modelNode1 = std::make_shared<StdMaterial>(m_matTexPhong, model1);
    modelNode1->SetBaseTexture(m_TextureCube);
    auto modelNode2 = std::make_shared<StdMaterial>(m_matTexPhong, model2);
    modelNode2->SetBaseTexture(m_TextureCube);

    auto matModel = dg::float4x4::Scale(1, 1, 1);
    m_scene->NewChild(modelNode1, matModel);
    matModel = dg::float4x4::Translation(1, 1, 1);
    m_scene->NewChild(modelNode2, matModel);
}
