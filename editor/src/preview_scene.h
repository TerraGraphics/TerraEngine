#pragma once

#include <memory>

#include "core/dg/dg.h"
#include "core/dg/math.h"
#include "core/common/ctor.h"


class Scene;
class Camera;
class Gizmo3D;
class Material;
class TransformNode;
class PreviewScene : Fixed {
public:
    PreviewScene();
    ~PreviewScene();

public:
    void Create();
    void Update(double deltaTime, const std::shared_ptr<Camera>& camera);
    void Draw();

    void SelectNode(uint32_t id);
    void SetMouseRay(dg::float3 rayStart, dg::float3 rayDir);
    void SetSpherePos(dg::float3 pos);

private:
    void CreateTextures();
    void CreateMaterials();
    void GenerateMeshes();
    void GenerateGizmo();

private:
    // textures
    TextureViewPtr m_TextureCube;

    // materials
    std::shared_ptr<Material> m_matTexNoLight;
    std::shared_ptr<Material> m_matTexDiscardNoLight;
    std::shared_ptr<Material> m_matTexPhong;
    std::shared_ptr<Material> m_matClrNoLight;
    std::shared_ptr<Material> m_matClrPhong;

    std::unique_ptr<Gizmo3D> m_gizmo;
    std::shared_ptr<Scene> m_scene;
    std::shared_ptr<TransformNode> m_sphere;

    uint32_t m_selectedId = std::numeric_limits<uint32_t>::max();
    bool m_findId = false;

    DevicePtr m_device;
};
