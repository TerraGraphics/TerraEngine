#pragma once


#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/scene/transform_graph.h"


class Scene : public TransformGraph, Fixed {
public:
    Scene() = delete;
    Scene(uint16_t vDeclIdPerInstance);
    ~Scene() = default;

    TransformUpdateDesc& Update(uint32_t findId);
    uint32_t Draw(ContextPtr& context);

private:
    uint16_t m_vDeclIdPerInstance;
    TransformUpdateDesc m_updateDesc;
};
