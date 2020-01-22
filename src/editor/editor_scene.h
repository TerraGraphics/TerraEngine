#pragma once

#include <memory>

#include "core/common/dg.h"
#include "core/common/ctor.h"


class Scene;
class Material;
class EditorScene : Fixed {
public:
    EditorScene() = default;
    ~EditorScene() = default;

public:
    void Create();
    void Update(double deltaTime);
    void Draw();

private:
    void CreateTextures();
    void CreateMaterials();
    void GenerateCube();

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
