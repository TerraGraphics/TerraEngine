#include "preview_scene.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/engine.h"
#include "core/dg/context.h" // IWYU pragma: keep
#include "core/math/types.h"
#include "core/dg/texture.h"
#include "core/scene/scene.h"
#include "core/scene/vertexes.h"
#include "core/dg/graphics_types.h"
#include "core/material/vdecl_item.h"
#include "core/dg/rasterizer_state.h"
#include "core/dg/texture_utilities.h"
#include "core/material/vdecl_storage.h"
#include "core/material/material_builder.h"
#include "middleware/generator/mesh_generator.h"
#include "middleware/std_material/std_material.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/noise_pojection.h"
#include "middleware/generator/texture/noise_rasterization.h"
#include "middleware/generator/texture/texture_node_factory.h"


PreviewScene::PreviewScene() {

}

PreviewScene::~PreviewScene() {

}

void PreviewScene::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_scene = std::make_shared<Scene>(m_device, engine.GetContext(), true);

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

    auto factory = std::make_unique<TextureNodeFactory>(m_device, Engine::Get().GetContext());
    auto* cNoise = factory->CreateCoherentNoise();
    auto* planePr = factory->CreatePlaneProjection()->SetInputs(cNoise);
    auto* texGen = factory->CreateNoiseToTexture()->SetInputs(planePr);
    m_TextureNoise = texGen->Get()->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
}

void PreviewScene::CreateMaterials() {
    auto& engine = Engine::Get();
    auto& materialBuilder = engine.GetMaterialBuilder();
    const auto BASE_COLOR_MATERIAL = materialBuilder->GetShaderMask("BASE_COLOR_MATERIAL");
    const auto BASE_COLOR_TEXTURE = materialBuilder->GetShaderMask("BASE_COLOR_TEXTURE");
    const auto ALPHA_TEST = materialBuilder->GetShaderMask("ALPHA_TEST");
    const auto AMBIENT_DIFFUSE_PHONG = materialBuilder->GetShaderMask("AMBIENT_DIFFUSE_PHONG");
    const auto COLOR_PICKER = materialBuilder->GetShaderMask("COLOR_PICKER");
    const auto vDeclIdPNC = VertexPNC::GetVDeclId();
    const auto vDeclIdPerInstance = engine.GetVDeclStorage()->Add({
        VDeclItem("WorldRow0", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow1", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow2", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow3", VDeclType::Float4, 1, false),
        VDeclItem("NormalRow0", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow1", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow2", VDeclType::Float3, 1, false),
        VDeclItem("IdColor", VDeclType::Color4, 1, false),
    });

    m_matTexNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE | COLOR_PICKER, vDeclIdPNC, vDeclIdPerInstance).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::noLight");

    m_matTexDiscardNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE | ALPHA_TEST | COLOR_PICKER, vDeclIdPNC, vDeclIdPerInstance).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::discard::noLight");

    m_matTexPhong = materialBuilder->Create(BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG | COLOR_PICKER, vDeclIdPNC, vDeclIdPerInstance).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::phong");

    m_matClrNoLight = materialBuilder->Create(BASE_COLOR_MATERIAL | COLOR_PICKER, vDeclIdPNC, vDeclIdPerInstance).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::noLight");

    m_matClrPhong = materialBuilder->Create(BASE_COLOR_MATERIAL | AMBIENT_DIFFUSE_PHONG | COLOR_PICKER, vDeclIdPNC, vDeclIdPerInstance).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::phong");
}

void PreviewScene::GenerateMeshes() {
    // TorusShape shape1(0.25f, 1.f, 10, 30, math::Axis::Y);
    // CubeShape shape1({1.f, 2.f, 3.f}, {2, 3, 4});
    // ConeShape shape1({30, 30}, math::Axis::Y);
    // PlaneShape shape1({math::Axis::X, math::Axis::Z}, math::Direction::POS_Y);
    PlaneShape shape1({math::Axis::Y, math::Axis::Z}, math::Direction::POS_X);
    SphereShape shape2({30, 30}, math::Axis::Y);
    auto model1 = ShapeBuilder(m_device).Join({&shape1}, "Model1");
    auto model2 = ShapeBuilder(m_device).Join({&shape2}, "Model2");

    auto modelNode1 = std::make_shared<StdMaterial>(m_matTexPhong);
    modelNode1->SetBaseTexture(m_TextureNoise);
    // modelNode1->SetBaseTexture(m_TextureCube);
    auto modelNode2 = std::make_shared<StdMaterial>(m_matTexPhong);
    modelNode2->SetBaseTexture(m_TextureCube);

    auto matModel = dg::float4x4::Scale(1, 1, 1);
    m_scene->NewChild(model1, modelNode1, matModel);
    matModel = dg::float4x4::Translation(1, 1, 1);
    m_scene->NewChild(model2, modelNode2, matModel);
}
