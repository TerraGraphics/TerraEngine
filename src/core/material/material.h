#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/PipelineState.h>

#include "core/common/dg.h"
#include "core/common/ctor.h"


class Material : Fixed {
public:
    Material() = delete;
    Material(const dg::RefCntAutoPtr<dg::IPipelineState>& pipelineState);

    dg::RefCntAutoPtr<dg::IPipelineState> GetPipelineState() { return m_pipelineState; }
    void Bind(ContextPtr& context);

private:
    dg::RefCntAutoPtr<dg::IPipelineState> m_pipelineState;
};