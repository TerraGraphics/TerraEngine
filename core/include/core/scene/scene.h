#pragma once


#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/scene/transform_graph.h"


class Scene : public TransformGraph, Fixed {
public:
    Scene() = default;
    ~Scene() = default;

    TransformUpdateDesc& Update(uint16_t targetsId, uint16_t vDeclIdPerInstance, uint32_t findId);
    uint32_t Draw(ContextPtr& context);

private:
    TransformUpdateDesc m_updateDesc;
};
