#pragma once

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"


class Material : public Counter<Material>, Fixed {
public:
    Material() = delete;
    Material(const PipelineStatePtr& pipelineState);

    const dg::Char* GetName() const;
    ShaderResourceBindingPtr CreateShaderResourceBinding();

    void Bind(ContextPtr& context);

private:
    PipelineStatePtr m_pipelineState;
};
