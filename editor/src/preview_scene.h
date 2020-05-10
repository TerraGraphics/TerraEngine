#pragma once

#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class Scene;
class TransformNode;
class StdMaterialNew;
class PreviewScene : Fixed {
public:
    PreviewScene();
    ~PreviewScene();

public:
    void Create();
    void AddChild(const std::shared_ptr<TransformNode>& node);
    std::shared_ptr<TransformNode> Update(double deltaTime, uint32_t findId = 0);
    void Draw();

private:
    void CreateTextures();
    void CreateMaterials();
    void GenerateMeshes();

private:
    // textures
    TextureViewPtr m_TextureCube;
    TextureViewPtr m_TextureNoise;

    // materials
    std::shared_ptr<StdMaterialNew> m_material0;
    std::shared_ptr<StdMaterialNew> m_material1;

    std::shared_ptr<Scene> m_scene;

    DevicePtr m_device;
};
