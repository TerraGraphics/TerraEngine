#pragma once

#include <memory>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class StdScene;
class StdMaterial;
class StdMaterialGrass;
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
    void GenerateGround();
    void GenerateTrees();
    void GenerateGrass();
    void GenerateGrassBillboard();

private:
    // textures
    TextureViewPtr m_TextureGround;
    TextureViewPtr m_TextureGrass0;
    TextureViewPtr m_TextureGrass1;
    TextureViewPtr m_TextureFlower0;
    TextureViewPtr m_TextureGrassBlade0;
    TextureViewPtr m_TextureGrassBlade1;

    // materials
    std::shared_ptr<StdMaterial> m_matGroud;
    std::shared_ptr<StdMaterial> m_matTrunk;
    std::shared_ptr<StdMaterial> m_matCrown;
    std::shared_ptr<StdMaterialGrass> m_matGrass;
    std::shared_ptr<StdMaterial> m_matGrassBillboard0;
    std::shared_ptr<StdMaterial> m_matGrassBillboard1;
    std::shared_ptr<StdMaterial> m_matGrassBillboard2;

    std::shared_ptr<StdScene> m_scene;

    DevicePtr m_device;
};
