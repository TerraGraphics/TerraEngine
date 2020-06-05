#include "editor/preview_scene.h"

#include "dg/texture.h"
#include "core/engine.h"
#include "core/math/types.h"
#include "dg/graphics_types.h"
#include "dg/rasterizer_state.h"
#include "dg/texture_utilities.h"
#include "middleware/std_render/std_scene.h"
#include "middleware/std_render/std_material.h"
#include "middleware/generator/mesh_generator.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/noise_pojection.h"
#include "middleware/generator/texture/noise_rasterization.h"
#include "middleware/generator/texture/texture_node_factory.h"


PreviewScene::PreviewScene() {

}

PreviewScene::~PreviewScene() {

}

void PreviewScene::Create(const std::shared_ptr<StdScene>& scene) {
    m_scene = scene;
    CreateTextures();
    CreateMaterials();
    GenerateMeshes();
}

void PreviewScene::AddChild(const std::shared_ptr<TransformNode>& node) {
    m_scene->AddChild(node);
}

void PreviewScene::CreateTextures() {
    auto& device = Engine::Get().GetDevice();

    dg::TextureLoadInfo loadInfo;
    loadInfo.IsSRGB = true;

    {
        TexturePtr Tex;
        CreateTextureFromFile("assets/ground.jpg", loadInfo, device, &Tex);
        m_TextureCube = Tex->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
    }

    auto factory = std::make_unique<TextureNodeFactory>();
    auto* cNoise = factory->CreateCoherentNoise();
    auto* planePr = factory->CreatePlaneProjection()->SetInputs(cNoise);
    auto* texGen = factory->CreateNoiseToTexture()->SetInputs(planePr);
    m_TextureNoise = texGen->Get()->GetDefaultView(dg::TEXTURE_VIEW_SHADER_RESOURCE);
}

void PreviewScene::CreateMaterials() {
    m_material0 = std::make_shared<StdMaterial>("mat::phong::node0");
    m_material0->SetCullMode(dg::CULL_MODE_NONE);
    m_material0->AmbientDiffuse(true);
    m_material0->SetBaseTexture(m_TextureNoise);

    m_material1 = std::make_shared<StdMaterial>("mat::phong::node1");
    m_material1->SetCullMode(dg::CULL_MODE_NONE);
    m_material1->AmbientDiffuse(true);
    m_material1->SetBaseTexture(m_TextureCube);
}

void PreviewScene::GenerateMeshes() {
    auto& device = Engine::Get().GetDevice();

    // TorusShape shape1(0.25f, 1.f, 10, 30, math::Axis::Y);
    // CubeShape shape1({1.f, 2.f, 3.f}, {2, 3, 4});
    // ConeShape shape1({30, 30}, math::Axis::Y);
    // PlaneShape shape1({math::Axis::X, math::Axis::Z}, math::Direction::POS_Y);
    PlaneShape shape1({math::Axis::Y, math::Axis::Z}, math::Direction::POS_X);
    SphereShape shape2({30, 30}, math::Axis::Y);
    auto model1 = ShapeBuilder(device).Join({&shape1}, "Model1");
    auto model2 = ShapeBuilder(device).Join({&shape2}, "Model2");

    auto matModel = dg::float4x4::Scale(1, 1, 1);
    m_scene->NewChild(model1, m_material0, matModel);
    matModel = dg::float4x4::Translation(1, 1, 1);
    m_scene->NewChild(model2, m_material1, matModel);
}
