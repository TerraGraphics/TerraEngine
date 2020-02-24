#pragma once

#include <memory>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class Scene;
class Material;
class TransformNode;
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

    // materials
    std::shared_ptr<Material> m_matTexNoLight;
    std::shared_ptr<Material> m_matTexDiscardNoLight;
    std::shared_ptr<Material> m_matTexPhong;
    std::shared_ptr<Material> m_matClrNoLight;
    std::shared_ptr<Material> m_matClrPhong;

    std::shared_ptr<Scene> m_scene;

    DevicePtr m_device;
};
