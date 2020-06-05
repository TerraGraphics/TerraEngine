#pragma once

#include <memory>

#include "dg/dg.h"
#include "core/common/ctor.h"


class StdScene;
class StdMaterial;
class TransformNode;
class PreviewScene : Fixed {
public:
    PreviewScene();
    ~PreviewScene();

public:
    void Create(const std::shared_ptr<StdScene>& scene);
    void AddChild(const std::shared_ptr<TransformNode>& node);

private:
    void CreateTextures();
    void CreateMaterials();
    void GenerateMeshes();

private:
    // textures
    TextureViewPtr m_TextureCube;
    TextureViewPtr m_TextureNoise;

    // materials
    std::shared_ptr<StdMaterial> m_material0;
    std::shared_ptr<StdMaterial> m_material1;

    std::shared_ptr<StdScene> m_scene;
};
