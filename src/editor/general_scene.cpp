#include "editor/general_scene.h"

#include <DiligentTools/TextureLoader/interface/TextureUtilities.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/ShaderMacroHelper.h>

#include "core/engine.h"
#include "core/scene/scene.h"
#include "core/math/random.h"
#include "core/scene/material_node.h"
#include "core/material/material_builder.h"
#include "middleware/generator/plane_shape.h"
#include "middleware/generator/shape_builder.h"
#include "middleware/generator/mesh_generator.h"


static dg::LayoutElement LayoutElems[] = {
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
    dg::LayoutElement{7, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow1
    dg::LayoutElement{8, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow2
    dg::LayoutElement{9, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow3
    dg::LayoutElement{10, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE}
};

static dg::InputLayoutDesc layoutDesc(LayoutElems, _countof(LayoutElems));


void GeneralScene::Create(std::shared_ptr<MaterialBuilder>& materialBuilder) {
    auto& engine = Engine::Get();
    m_device = engine.GetDevice();
    m_scene = std::make_shared<Scene>(m_device, engine.GetImmediateContext());

    CreateShaders();
    CreateTextures();
    CreateMaterials(materialBuilder);
    GenerateGround();
    GenerateTrees();
    GenerateGrass();
}

void GeneralScene::Update(double deltaTime) {
    m_scene->Update();
}

void GeneralScene::Draw() {
    m_scene->Draw();
}

void GeneralScene::CreateShaders() {
    dg::ShaderCreateInfo ShaderCI;
    ShaderCI.SourceLanguage = dg::SHADER_SOURCE_LANGUAGE_HLSL;
    ShaderCI.UseCombinedTextureSamplers = true;

    dg::RefCntAutoPtr<dg::IShaderSourceInputStreamFactory> pShaderSourceFactory;
    Engine::Get().GetEngineFactory()->CreateDefaultShaderSourceStreamFactory("materials", &pShaderSourceFactory);
    ShaderCI.pShaderSourceStreamFactory = pShaderSourceFactory;
    ShaderCI.CombinedSamplerSuffix = "Sampler";

    {
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_VERTEX;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "vs::standart";
        ShaderCI.FilePath        = "standart.vsh";
        m_device->CreateShader(ShaderCI, &m_vsStandart);
    }
    {
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "ps::tex::noLight";
        ShaderCI.FilePath        = "noLight.psh";
        m_device->CreateShader(ShaderCI, &m_psTexNoLight);
    }
    {
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "ps::tex::discard::noLight";
        ShaderCI.FilePath        = "noLight_discard.psh";
        m_device->CreateShader(ShaderCI, &m_psTexDiscardNoLight);
    }
    {
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "ps::tex::phong";
        ShaderCI.FilePath        = "phong.psh";
        m_device->CreateShader(ShaderCI, &m_psTexPhong);
    }
    {
        dg::ShaderMacroHelper Macros;
        Macros.AddShaderMacro("DISABLE_TEXTURE", 1);
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "ps::clr::noLight";
        ShaderCI.FilePath        = "noLight.psh";
        ShaderCI.Macros          = Macros;
        m_device->CreateShader(ShaderCI, &m_psClrNoLight);
        ShaderCI.Macros = nullptr;
    }
    {
        dg::ShaderMacroHelper Macros;
        Macros.AddShaderMacro("DISABLE_TEXTURE", 1);
        ShaderCI.Desc.ShaderType = dg::SHADER_TYPE_PIXEL;
        ShaderCI.EntryPoint      = "main";
        ShaderCI.Desc.Name       = "ps::clr::phong";
        ShaderCI.FilePath        = "phong.psh";
        ShaderCI.Macros          = Macros;
        m_device->CreateShader(ShaderCI, &m_psClrPhong);
        ShaderCI.Macros = nullptr;
    }
}

void GeneralScene::CreateTextures() {
    dg::TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;

    {
        dg::RefCntAutoPtr<dg::ITexture> Tex;
        CreateTextureFromFile("assets/ground.jpg", loadInfo, m_device, &Tex);
        m_TextureGround = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        dg::RefCntAutoPtr<dg::ITexture> Tex;
        CreateTextureFromFile("assets/grass0.png", loadInfo, m_device, &Tex);
        m_TextureGrass0 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        dg::RefCntAutoPtr<dg::ITexture> Tex;
        CreateTextureFromFile("assets/grass1.png", loadInfo, m_device, &Tex);
        m_TextureGrass1 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
    {
        dg::RefCntAutoPtr<dg::ITexture> Tex;
        CreateTextureFromFile("assets/flower0.png", loadInfo, m_device, &Tex);
        m_TextureFlower0 = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }
}

void GeneralScene::CreateMaterials(std::shared_ptr<MaterialBuilder>& materialBuilder) {
    m_matTexNoLight = materialBuilder->Create(m_vsStandart, m_psTexNoLight, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_VERTEX, "Transform", dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::noLight");

    m_matTexDiscardNoLight = materialBuilder->Create(m_vsStandart, m_psTexDiscardNoLight, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_VERTEX, "Transform", dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_CLAMP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::discard::noLight");

    m_matTexPhong = materialBuilder->Create(m_vsStandart, m_psTexPhong, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_VERTEX, "Transform", dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC).
        TextureVar(dg::SHADER_TYPE_PIXEL, "texBase", dg::TEXTURE_ADDRESS_WRAP, dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::tex::phong");

    m_matClrNoLight = materialBuilder->Create(m_vsStandart, m_psClrNoLight, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_VERTEX, "Transform", dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::noLight");

    m_matClrPhong = materialBuilder->Create(m_vsStandart, m_psClrPhong, layoutDesc).
        CullMode(dg::CULL_MODE_NONE).
        Var(dg::SHADER_TYPE_VERTEX, "Transform", dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC).
        Var(dg::SHADER_TYPE_PIXEL, "Material", dg::SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE).
        Build("mat::clr::phong");
}

void GeneralScene::GenerateGround() {
    auto plane = MeshGenerator::CreateSolidPlaneXZ(m_device, 2, 2, 4.0f, 4.0f);

    auto groundNode = std::make_shared<MaterialNode>(m_matTexNoLight, plane);
    groundNode->SetPixelShaderVar("texBase", m_TextureGround);

    auto matModel = dg::float4x4::Scale(256, 1, 256);
    m_scene->NewChild(groundNode, matModel);
}

void GeneralScene::GenerateTrees() {
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

void GeneralScene::GenerateGrass() {
    float angleY = ThirdPI<float>() * 2.0f;
    auto matBase = dg::float4x4::Translation(0, 0.5, 0) * dg::float4x4::RotationX(ThirdPI<float>() / 3.f);

    PlaneShape plane1(2, 2, 1.0f, 1.0f, Axis::Z);
    plane1.SetTranform(matBase);

    PlaneShape plane2(2, 2, 1.0f, 1.0f, Axis::Z);
    plane2.SetTranform(matBase * dg::float4x4::RotationY(angleY));

    PlaneShape plane3(2, 2, 1.0f, 1.0f, Axis::Z);
    plane3.SetTranform(matBase * dg::float4x4::RotationY(angleY * 2.f));

    auto bush = ShapeBuilder(m_device).Join({&plane1, &plane2, &plane3}, "Bush");

    auto grass0MatNode = std::make_shared<MaterialNode>(m_matTexDiscardNoLight, bush);
    grass0MatNode->SetPixelShaderVar("texBase", m_TextureGrass0);

    auto grass1MatNode = std::make_shared<MaterialNode>(m_matTexDiscardNoLight, bush);
    grass1MatNode->SetPixelShaderVar("texBase", m_TextureGrass1);

    auto flower0MatNode = std::make_shared<MaterialNode>(m_matTexDiscardNoLight, bush);
    flower0MatNode->SetPixelShaderVar("texBase", m_TextureFlower0);

    std::srand(15);
    auto materialNode = grass0MatNode;
    auto multiplier = 10;
    for (int i=0; i!=multiplier * 1000; ++i) {
        auto matScale = dg::float4x4::Scale(LinearRand(0.3f, 1.f));

        auto vecPos = LinearRand(dg::float3(-50.f, 0.f, -50.f), dg::float3(50.f, 0.f, 50.f));
        auto matModelPosition = dg::float4x4::Translation(vecPos);

        if (i == multiplier * 450) {
            materialNode = grass1MatNode;
        } else  if (i == multiplier * 900) {
            materialNode = flower0MatNode;
        }
        m_scene->NewChild(materialNode, matScale * matModelPosition);
    }
}
