#pragma once

#include "core/scene/transform_graph.h"


class WriteableVertexBuffer;
class Scene : public TransformGraph, Fixed {
public:
    Scene() = delete;
    Scene(DevicePtr device, ContextPtr context);
    ~Scene() = default;

    void Update();
    uint32_t Draw();

private:
    DevicePtr m_device;
    ContextPtr m_context;
    std::shared_ptr<WriteableVertexBuffer> m_transformBuffer;
    std::vector<std::shared_ptr<TransformNode>> m_nodeListForDraw;
};