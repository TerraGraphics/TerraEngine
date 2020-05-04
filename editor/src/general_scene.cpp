#include "general_scene.h"

#include <cstdint>

#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/engine.h"
#include "core/dg/context.h" // IWYU pragma: keep
#include "core/dg/texture.h"
#include "core/math/types.h"
#include "core/scene/scene.h"
#include "core/math/random.h"
#include "core/scene/vertexes.h"
#include "core/math/constants.h"
#include "core/scene/geometry.h"
#include "core/dg/graphics_types.h"
#include "core/scene/vertex_buffer.h"
#include "core/dg/rasterizer_state.h"
#include "core/material/vdecl_item.h"
#include "core/dg/texture_utilities.h"
#include "core/scene/transform_graph.h"
#include "core/material/vdecl_storage.h"
#include "core/material/material_builder.h"
#include "middleware/generator/mesh_generator.h"
#include "middleware/std_material/std_material.h"


void GeneralScene::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_scene = std::make_shared<Scene>(m_device, engine.GetContext(), false);

    CreateTextures();
    CreateMaterials();
    GenerateGround();
    GenerateTrees();
    GenerateGrass();
    // GenerateGrassBillboard();
}

void GeneralScene::Update(double /* deltaTime */) {
    m_scene->Update();
}

void GeneralScene::Draw() {
    m_scene->Draw();
}

void GeneralScene::CreateTextures() {
    dg::TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;

    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/ground.jpg", loadInfo, m_device, &Tex);
        m_TextureGround = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/grass0.png", loadInfo, m_device, &Tex);
        m_TextureGrass0 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/grass1.png", loadInfo, m_device, &Tex);
        m_TextureGrass1 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/flower0.png", loadInfo, m_device, &Tex);
        m_TextureFlower0 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/blade_0.png", loadInfo, m_device, &Tex);
        m_TextureGrassBlade0 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/blade_1.jpg", loadInfo, m_device, &Tex);
        m_TextureGrassBlade1 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
}

void GeneralScene::CreateMaterials() {
    auto& engine = Engine::Get();
    auto& materialBuilder = engine.GetMaterialBuilder();
    const auto BASE_COLOR_MATERIAL = materialBuilder->GetShaderMask("BASE_COLOR_MATERIAL");
    const auto BASE_COLOR_TEXTURE = materialBuilder->GetShaderMask("BASE_COLOR_TEXTURE");
    const auto ALPHA_TEST = materialBuilder->GetShaderMask("ALPHA_TEST");
    const auto AMBIENT_DIFFUSE_PHONG = materialBuilder->GetShaderMask("AMBIENT_DIFFUSE_PHONG");
    const auto GRASS = materialBuilder->GetShaderMask("GRASS");
    const auto vDeclPNC = VertexPNC::GetVDeclId();
    const auto vDeclGrass = VertexP::GetVDeclId();
    const auto vDeclinstance = engine.GetVDeclStorage()->Add({
        VDeclItem("WorldRow0", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow1", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow2", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow3", VDeclType::Float4, 1, false),
        VDeclItem("NormalRow0", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow1", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow2", VDeclType::Float3, 1, false),
    });

    m_matTexNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE, vDeclPNC, vDeclinstance).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::noLight");

    m_matTexDiscardNoLight = materialBuilder->Create(BASE_COLOR_TEXTURE | ALPHA_TEST, vDeclPNC, vDeclinstance).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::discard::noLight");

    m_matTexPhong = materialBuilder->Create(BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG, vDeclPNC, vDeclinstance).
        CullMode(dg::CULL_MODE_NONE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::phong");

    m_matClrNoLight = materialBuilder->Create(BASE_COLOR_MATERIAL, vDeclPNC, vDeclinstance).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::noLight");

    m_matClrPhong = materialBuilder->Create(BASE_COLOR_MATERIAL | AMBIENT_DIFFUSE_PHONG, vDeclPNC, vDeclinstance).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::phong");

    m_matGrass = materialBuilder->Create(GRASS | BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG, vDeclGrass, vDeclinstance).
        CullMode(dg::CULL_MODE_NONE).
        Topology(dg::PRIMITIVE_TOPOLOGY_POINT_LIST).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::grass");

    m_matGrassAlpha = materialBuilder->Create(GRASS | BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG | ALPHA_TEST, vDeclGrass, vDeclinstance).
        CullMode(dg::CULL_MODE_NONE).
        Topology(dg::PRIMITIVE_TOPOLOGY_POINT_LIST).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::grass::alpha");
}

void GeneralScene::GenerateGround() {
    PlaneShape shape({math::Axis::X, math::Axis::Z}, math::Direction::POS_Y);
    shape.SetUVScale({128, 128});
    auto plane = ShapeBuilder(m_device).Join({&shape}, "Ground");

    auto groundNode = std::make_shared<StdMaterial>(m_matTexNoLight);
    groundNode->SetBaseTexture(m_TextureGround);

    auto matModel = dg::float4x4::Scale(256, 1, 256);
    m_scene->NewChild(plane, groundNode, matModel);
}

void GeneralScene::GenerateTrees() {
    RandSeed(17);
    auto tree = std::make_shared<TransformNode>();

    CylinderShape trunkShape({5, 1}, math::Axis::Y);
    auto trunkGeometry = ShapeBuilder(m_device).Join({&trunkShape}, "trunk");

    auto trunkMatNode = std::make_shared<StdMaterial>(m_matClrPhong);
    trunkMatNode->SetBaseColor(139, 69, 19);

    auto matModelTrunk = dg::float4x4::Scale(0.5, 4, 0.5) * dg::float4x4::Translation(0, 2, 0);
    tree->NewChild(trunkGeometry, trunkMatNode, matModelTrunk);

    SphereShape crownShape({10, 5}, math::Axis::Y);
    auto crownGeometry = ShapeBuilder(m_device).Join({&crownShape}, "crown");
    auto crownMatNode = std::make_shared<StdMaterial>(m_matClrPhong);
    crownMatNode->SetBaseColor(0, 128, 0);

    auto matModelCrown = dg::float4x4::Scale(4, 8, 4) * dg::float4x4::Translation(0, 7, 0);
    tree->NewChild(crownGeometry, crownMatNode, matModelCrown);

    RandSeed(5);
    for (auto i=0; i!=100; ++i) {
        auto matModelPosition = dg::float4x4::Translation(LinearRand(dg::float3(-100, 0, -100), dg::float3(100, 0, 100)));
        m_scene->AddChild(tree->Clone(matModelPosition));
    }
}

void GeneralScene::GenerateGrass() {
    RandSeed(177);

    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexP>(1 * 10 * 1000);
    float halfWidt = 150.f;
    for (auto* vbIt = vb.Begin(); vbIt != vb.End(); ++vbIt) {
        auto vecPos = LinearRand(dg::float3(-halfWidt, 0.f, -halfWidt), dg::float3(halfWidt, 0.f, halfWidt));
        *vbIt = VertexP{vecPos};
    }

    uint32_t vbOffsetBytes = 0;
    auto geometry = std::make_shared<GeometryUnindexed>(vbBuilder.Build(m_device, "grass points"), vbOffsetBytes, vb.Count());

    auto grassMatNode = std::make_shared<StdMaterial>(m_matGrass);
    grassMatNode->SetBaseTexture(m_TextureGrassBlade1);

    // auto grassMatNode = std::make_shared<StdMaterial>(m_matGrassAlpha, geometry);
    // grassMatNode->SetAlphaThreshold(0.2f);
    // grassMatNode->SetBaseTexture(m_TextureGrassBlade0);

    m_scene->NewChild(geometry, grassMatNode);
}

void GeneralScene::GenerateGrassBillboard() {
    float angleY = OneThirdOfPI<float>() * 2.0f;
    auto matBase = dg::float4x4::Translation(0, 0.5, 0) * dg::float4x4::RotationX(OneThirdOfPI<float>() / 3.f);

    PlaneShape plane1({math::Axis::X, math::Axis::Y}, math::Direction::POS_Z);
    plane1.SetTransform(matBase);

    PlaneShape plane2({math::Axis::X, math::Axis::Y}, math::Direction::POS_Z);
    plane2.SetTransform(matBase * dg::float4x4::RotationY(angleY));

    PlaneShape plane3({math::Axis::X, math::Axis::Y}, math::Direction::POS_Z);
    plane3.SetTransform(matBase * dg::float4x4::RotationY(angleY * 2.f));

    auto bush = ShapeBuilder(m_device).Join({&plane1, &plane2, &plane3}, "Bush");

    auto grass0MatNode = std::make_shared<StdMaterial>(m_matTexDiscardNoLight);
    grass0MatNode->SetAlphaThreshold(0.2f);
    grass0MatNode->SetBaseTexture(m_TextureGrass0);

    auto grass1MatNode = std::make_shared<StdMaterial>(m_matTexDiscardNoLight);
    grass1MatNode->SetAlphaThreshold(0.2f);
    grass1MatNode->SetBaseTexture(m_TextureGrass1);

    auto flower0MatNode = std::make_shared<StdMaterial>(m_matTexDiscardNoLight);
    flower0MatNode->SetAlphaThreshold(0.2f);
    flower0MatNode->SetBaseTexture(m_TextureFlower0);

    RandSeed(15);
    auto material = grass0MatNode;
    auto multiplier = 10;
    for (int i=0; i!=multiplier * 1000; ++i) {
        auto matScale = dg::float4x4::Scale(LinearRand(0.3f, 1.f));

        auto vecPos = LinearRand(dg::float3(-100.f, 0.f, -100.f), dg::float3(100.f, 0.f, 100.f));
        auto matModelPosition = dg::float4x4::Translation(vecPos);

        if (i == multiplier * 450) {
            material = grass1MatNode;
        } else  if (i == multiplier * 900) {
            material = flower0MatNode;
        }
        m_scene->NewChild(bush, material, matScale * matModelPosition);
    }
}
