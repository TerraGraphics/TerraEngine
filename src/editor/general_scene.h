#pragma once

#include <memory>

#include "core/common/dg.h"
#include "core/common/ctor.h"


namespace Diligent {
    class IShader;
    class ITextureView;
}

class Scene;
class Material;
class GeneralScene : Fixed {
public:
    GeneralScene() = default;
    ~GeneralScene() = default;

public:
    void Create();
    void Update(double deltaTime);
    void Draw();

private:
    void CreateTextures();
    void CreateMaterials();
    void GenerateGround();
    void GenerateTrees();
    void GenerateGrass();
    void GenerateGrassBillboard();

private:
    // textures
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGround;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGrass0;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGrass1;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureFlower0;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGrassBlade0;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGrassBlade1;

    // materials
    std::shared_ptr<Material> m_matTexNoLight;
    std::shared_ptr<Material> m_matTexDiscardNoLight;
    std::shared_ptr<Material> m_matTexPhong;
    std::shared_ptr<Material> m_matClrNoLight;
    std::shared_ptr<Material> m_matClrPhong;
    std::shared_ptr<Material> m_matGrass;
    std::shared_ptr<Material> m_matGrassAlpha;

    std::shared_ptr<Scene> m_scene;

    DevicePtr m_device;
};
