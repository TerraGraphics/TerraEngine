#include "editor/general_scene.h"

#include <DiligentTools/TextureLoader/interface/TextureUtilities.h>

#include "core/engine.h"
#include "core/scene/scene.h"
#include "core/math/random.h"
#include "platforms/platforms.h"
#include "core/scene/material_node.h"
#include "core/scene/geometry_node.h"
#include "core/material/material_builder.h"
#include "middleware/generator/plane_shape.h"
#include "middleware/generator/shape_builder.h"
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

static dg::LayoutElement layoutGrassElems[] = {
    // vertex position
    dg::LayoutElement{0, 0, 3, dg::VT_FLOAT32, dg::False},

    // Per-instance data - second buffer slot
    // WorldRow0
    dg::LayoutElement{1, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow1
    dg::LayoutElement{2, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow2
    dg::LayoutElement{3, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow3
    dg::LayoutElement{4, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow0
    dg::LayoutElement{5, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow1
    dg::LayoutElement{6, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow2
    dg::LayoutElement{7, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
};

static dg::InputLayoutDesc layoutGrassDesc(layoutGrassElems, _countof(layoutGrassElems));


void GeneralScene::Create() {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_scene = std::make_shared<Scene>(m_device, engine.GetImmediateContext());

    CreateTextures();
    CreateMaterials();
    GenerateGround();
    GenerateTrees();
    GenerateGrass();
    // GenerateGrassBillboard();
}

void GeneralScene::Update(double deltaTime) {
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

    m_matGrass = materialBuilder->Create(GRASS | BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG, layoutGrassDesc).
        CullMode(dg::CULL_MODE_NONE).
        Topology(dg::PRIMITIVE_TOPOLOGY_POINT_LIST).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::grass");

    m_matGrassAlpha = materialBuilder->Create(GRASS | BASE_COLOR_TEXTURE | AMBIENT_DIFFUSE_PHONG | ALPHA_TEST, layoutGrassDesc).
        CullMode(dg::CULL_MODE_NONE).
        Topology(dg::PRIMITIVE_TOPOLOGY_POINT_LIST).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::grass::alpha");
}

void GeneralScene::GenerateGround() {
    auto plane = MeshGenerator::CreateSolidPlaneXZ(m_device, 2, 2, 128.0f, 128.0f);

    auto groundNode = std::make_shared<MaterialNode>(m_matTexNoLight, plane);
    groundNode->SetPixelShaderVar("texBase", m_TextureGround);

    auto matModel = dg::float4x4::Scale(256, 1, 256);
    m_scene->NewChild(groundNode, matModel);
}

void GeneralScene::GenerateTrees() {
    RandSeed(17);
    auto tree = std::make_shared<TransformNode>();

    auto trunkMatNode = std::make_shared<MaterialClrNode>(m_matClrPhong, MeshGenerator::CreateSolidCylinder(m_device, 5));
    trunkMatNode->Params().crlBase = dg::float4(139.f, 69.f, 19.f, 255.f) / 255.f;

    auto matModelTrunk = dg::float4x4::Scale(0.5, 4, 0.5) * dg::float4x4::Translation(0, 2, 0);
    tree->NewChild(trunkMatNode, matModelTrunk);

    auto crownMatNode = std::make_shared<MaterialClrNode>(m_matClrPhong, MeshGenerator::CreateSolidSphere(m_device, 10));
    crownMatNode->Params().crlBase = dg::float4(0, 128.f, 0, 255.f) / 255.f;

    auto matModelCrown = dg::float4x4::Scale(4, 8, 4) * dg::float4x4::Translation(0, 7, 0);
    tree->NewChild(crownMatNode, matModelCrown);

    std::srand(5);
    for (auto i=0; i!=100; ++i) {
        auto matModelPosition = dg::float4x4::Translation(LinearRand(dg::float3(-100, 0, -100), dg::float3(100, 0, 100)));
        m_scene->AddChild(tree->Clone(matModelPosition));
    }
}

struct VertexP {
	dg::float3 position;
};

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
    auto geometryNode = std::make_shared<GeometryNodeUnindexed>(vbBuilder.Build(m_device, "grass points"), vbOffsetBytes, vb.Count());

    auto grassMatNode = std::make_shared<MaterialNode>(m_matGrass, geometryNode);
    grassMatNode->SetPixelShaderVar("texBase", m_TextureGrassBlade1);

    // auto grassMatNode = std::make_shared<MaterialClrNode>(m_matGrassAlpha, geometryNode);
    // grassMatNode->Params().alphaThreshold = 0.2f;
    // grassMatNode->SetPixelShaderVar("texBase", m_TextureGrassBlade0);

    m_scene->NewChild(grassMatNode);
}

void GeneralScene::GenerateGrassBillboard() {
    float angleY = ThirdPI<float>() * 2.0f;
    auto matBase = dg::float4x4::Translation(0, 0.5, 0) * dg::float4x4::RotationX(ThirdPI<float>() / 3.f);

    PlaneShape plane1(2, 2, 1.0f, 1.0f, Axis::Z);
    plane1.SetTranform(matBase);

    PlaneShape plane2(2, 2, 1.0f, 1.0f, Axis::Z);
    plane2.SetTranform(matBase * dg::float4x4::RotationY(angleY));

    PlaneShape plane3(2, 2, 1.0f, 1.0f, Axis::Z);
    plane3.SetTranform(matBase * dg::float4x4::RotationY(angleY * 2.f));

    auto bush = ShapeBuilder(m_device).Join({&plane1, &plane2, &plane3}, "Bush");

    auto grass0MatNode = std::make_shared<MaterialClrNode>(m_matTexDiscardNoLight, bush);
    grass0MatNode->Params().alphaThreshold = 0.2f;
    grass0MatNode->SetPixelShaderVar("texBase", m_TextureGrass0);

    auto grass1MatNode = std::make_shared<MaterialClrNode>(m_matTexDiscardNoLight, bush);
    grass1MatNode->Params().alphaThreshold = 0.2f;
    grass1MatNode->SetPixelShaderVar("texBase", m_TextureGrass1);

    auto flower0MatNode = std::make_shared<MaterialClrNode>(m_matTexDiscardNoLight, bush);
    flower0MatNode->Params().alphaThreshold = 0.2f;
    flower0MatNode->SetPixelShaderVar("texBase", m_TextureFlower0);

    RandSeed(15);
    auto materialNode = grass0MatNode;
    auto multiplier = 10;
    for (int i=0; i!=multiplier * 1000; ++i) {
        auto matScale = dg::float4x4::Scale(LinearRand(0.3f, 1.f));

        auto vecPos = LinearRand(dg::float3(-100.f, 0.f, -100.f), dg::float3(100.f, 0.f, 100.f));
        auto matModelPosition = dg::float4x4::Translation(vecPos);

        if (i == multiplier * 450) {
            materialNode = grass1MatNode;
        } else  if (i == multiplier * 900) {
            materialNode = flower0MatNode;
        }
        m_scene->NewChild(materialNode, matScale * matModelPosition);
    }
}
