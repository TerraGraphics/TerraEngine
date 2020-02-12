#pragma once

#include <memory>
#include "core/dg/dg.h"
#include "core/common/ctor.h"


class TransformNode;
class MaterialBuilder;
class Gizmo3D : Fixed {
public:
    Gizmo3D() = default;
    ~Gizmo3D() = default;

public:
    std::shared_ptr<TransformNode> Create(DevicePtr& device, std::shared_ptr<MaterialBuilder>& materialBuilder);

private:
    std::shared_ptr<TransformNode> m_rootNode;
    std::shared_ptr<TransformNode> m_transformNode;
};
