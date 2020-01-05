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
class MaterialBuilder;
class GeneralScene : Fixed {
public:
    GeneralScene() = default;
    ~GeneralScene() = default;

public:
    void Create(std::shared_ptr<MaterialBuilder>& materialBuilder);
    void Update(double deltaTime);
    void Draw();

private:
    void CreateShaders();
    void CreateTextures();
    void CreateMaterials(std::shared_ptr<MaterialBuilder>& materialBuilder);
    void GenerateGround();
    void GenerateTrees();
    void GenerateGrass();

private:
    // shaders
    dg::RefCntAutoPtr<dg::IShader> m_vsStandart;
    dg::RefCntAutoPtr<dg::IShader> m_psTex;
    dg::RefCntAutoPtr<dg::IShader> m_psTexDiscard;
    dg::RefCntAutoPtr<dg::IShader> m_psClr;

    // textures
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGround;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGrass0;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureGrass1;
    dg::RefCntAutoPtr<dg::ITextureView> m_TextureFlower0;

    // materials
    std::shared_ptr<Material> m_materialTex;
    std::shared_ptr<Material> m_materialTexDiscard;
    std::shared_ptr<Material> m_materialClr;

    std::shared_ptr<Scene> m_scene;

    DevicePtr m_device;
};
