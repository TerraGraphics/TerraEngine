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
#include "middleware/std_material/std_material_new.h"


void GeneralScene::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    const auto vDeclIdPerInstance = engine.GetVDeclStorage()->Add({
        VDeclItem("WorldRow0", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow1", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow2", VDeclType::Float4, 1, false),
        VDeclItem("WorldRow3", VDeclType::Float4, 1, false),
        VDeclItem("NormalRow0", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow1", VDeclType::Float3, 1, false),
        VDeclItem("NormalRow2", VDeclType::Float3, 1, false),
    });
    m_scene = std::make_shared<Scene>(m_device, engine.GetContext(), vDeclIdPerInstance, false);

    CreateTextures();
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

void GeneralScene::GenerateGround() {
    m_matGroud = std::make_shared<StdMaterialNew>("mat::ground");
    m_matGroud->SetCullMode(dg::CULL_MODE_NONE);
    m_matGroud->SetBaseTexture(m_TextureGround);
    auto& desc = m_matGroud->GetBaseTextureDesc();
    desc.AddressU = dg::TEXTURE_ADDRESS_WRAP;
    desc.AddressV = dg::TEXTURE_ADDRESS_WRAP;

    PlaneShape shape({math::Axis::X, math::Axis::Z}, math::Direction::POS_Y);
    shape.SetUVScale({128, 128});
    auto plane = ShapeBuilder(m_device).Join({&shape}, "Ground");

    auto matModel = dg::float4x4::Scale(256, 1, 256);
    m_scene->NewChild(plane, m_matGroud, matModel);
}

void GeneralScene::GenerateTrees() {
    RandSeed(17);

    auto tree = std::make_shared<TransformNode>();

    // Trunk
    m_matTrunk = std::make_shared<StdMaterialNew>("mat::trunk");
    m_matTrunk->SetCullMode(dg::CULL_MODE_NONE);
    m_matTrunk->AmbientDiffuse(true);
    m_matTrunk->SetBaseColor(139, 69, 19);

    CylinderShape trunkShape({5, 1}, math::Axis::Y);
    auto trunkGeometry = ShapeBuilder(m_device).Join({&trunkShape}, "trunk");

    auto matModelTrunk = dg::float4x4::Scale(0.5, 4, 0.5) * dg::float4x4::Translation(0, 2, 0);
    tree->NewChild(trunkGeometry, m_matTrunk, matModelTrunk);

    // Crown
    m_matCrown = std::make_shared<StdMaterialNew>("mat::crown");
    m_matCrown->SetCullMode(dg::CULL_MODE_NONE);
    m_matCrown->AmbientDiffuse(true);
    m_matCrown->SetBaseColor(0, 128, 0);

    SphereShape crownShape({10, 5}, math::Axis::Y);
    auto crownGeometry = ShapeBuilder(m_device).Join({&crownShape}, "crown");

    auto matModelCrown = dg::float4x4::Scale(4, 8, 4) * dg::float4x4::Translation(0, 7, 0);
    tree->NewChild(crownGeometry, m_matCrown, matModelCrown);

    RandSeed(5);
    for (auto i=0; i!=100; ++i) {
        auto matModelPosition = dg::float4x4::Translation(LinearRand(dg::float3(-100, 0, -100), dg::float3(100, 0, 100)));
        m_scene->AddChild(tree->Clone(matModelPosition));
    }
}

void GeneralScene::GenerateGrass() {
    RandSeed(177);

    m_matGrass = std::make_shared<StdMaterialGrass>("mat::grass");
    m_matGrass->SetCullMode(dg::CULL_MODE_NONE);
    m_matGrass->SetTopology(dg::PRIMITIVE_TOPOLOGY_POINT_LIST);
    m_matGrass->AmbientDiffuse(true);
    m_matGrass->SetBaseTexture(m_TextureGrassBlade1);
    // m_matGrass->SetBaseTexture(m_TextureGrassBlade0);
    // m_matGrass->SetAlphaThreshold(0.2f);
    auto& desc = m_matGrass->GetBaseTextureDesc();
    desc.AddressU = dg::TEXTURE_ADDRESS_WRAP;
    desc.AddressV = dg::TEXTURE_ADDRESS_WRAP;

    VertexBufferBuilder vbBuilder;
    auto vb = vbBuilder.AddRange<VertexP>(1 * 10 * 1000);
    float halfWidt = 150.f;
    for (auto* vbIt = vb.Begin(); vbIt != vb.End(); ++vbIt) {
        auto vecPos = LinearRand(dg::float3(-halfWidt, 0.f, -halfWidt), dg::float3(halfWidt, 0.f, halfWidt));
        *vbIt = VertexP{vecPos};
    }

    uint32_t vbOffsetBytes = 0;
    auto geometry = std::make_shared<GeometryUnindexed>(vbBuilder.Build(m_device, "grass points"), vbOffsetBytes, vb.Count(), vb.GetVDeclId());

    m_scene->NewChild(geometry, m_matGrass);
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

    m_matGrassBillboard0 = std::make_shared<StdMaterialNew>("mat::grass0");
    m_matGrassBillboard0->SetCullMode(dg::CULL_MODE_NONE);
    m_matGrassBillboard0->SetBaseTexture(m_TextureGrass0);
    m_matGrassBillboard0->SetAlphaThreshold(0.2f);

    m_matGrassBillboard1 = std::make_shared<StdMaterialNew>("mat::grass1");
    m_matGrassBillboard1->SetCullMode(dg::CULL_MODE_NONE);
    m_matGrassBillboard1->SetBaseTexture(m_TextureGrass1);
    m_matGrassBillboard1->SetAlphaThreshold(0.2f);

    m_matGrassBillboard2 = std::make_shared<StdMaterialNew>("mat::flower");
    m_matGrassBillboard2->SetCullMode(dg::CULL_MODE_NONE);
    m_matGrassBillboard2->SetBaseTexture(m_TextureFlower0);
    m_matGrassBillboard2->SetAlphaThreshold(0.2f);

    RandSeed(15);
    auto material = m_matGrassBillboard0;
    auto multiplier = 10;
    for (int i=0; i!=multiplier * 1000; ++i) {
        auto matScale = dg::float4x4::Scale(LinearRand(0.3f, 1.f));

        auto vecPos = LinearRand(dg::float3(-100.f, 0.f, -100.f), dg::float3(100.f, 0.f, 100.f));
        auto matModelPosition = dg::float4x4::Translation(vecPos);

        if (i == multiplier * 450) {
            material = m_matGrassBillboard1;
        } else  if (i == multiplier * 900) {
            material = m_matGrassBillboard2;
        }
        m_scene->NewChild(bush, material, matScale * matModelPosition);
    }
}
