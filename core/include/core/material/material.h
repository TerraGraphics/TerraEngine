#pragma once

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/counter.h"


class Material : public Counter<Material>, Fixed {
public:
    Material() = delete;
    Material(const PipelineStatePtr& pipelineState, uint32_t vDeclId);

    const char* GetName() const;
    uint32_t GetVDeclId() const noexcept { return m_vDeclId; }

    ShaderResourceBindingPtr CreateShaderResourceBinding();

    void Bind(ContextPtr& context);

private:
    uint32_t m_vDeclId;
    PipelineStatePtr m_pipelineState;
};
