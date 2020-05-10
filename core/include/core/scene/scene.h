#pragma once


#include <memory>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/scene/transform_graph.h"


class WriteableVertexBuffer;
class Scene : public TransformGraph, Fixed {
public:
    Scene() = delete;
    Scene(DevicePtr device, ContextPtr context, uint16_t vDeclIdPerInstance, bool addId);
    ~Scene() = default;

    std::shared_ptr<TransformNode> Update(uint32_t findId = 0);
    uint32_t Draw();

private:
    DevicePtr m_device;
    ContextPtr m_context;
    uint16_t m_vDeclIdPerInstance;
    bool m_addId = false;
    std::shared_ptr<WriteableVertexBuffer> m_transformBuffer;
    uint32_t m_transformBufferBufferElementNumber = 0;
    TransformUpdateDesc m_updateDesc;
};
