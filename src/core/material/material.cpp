#include "core/material/material.h"


Material::Material(const dg::RefCntAutoPtr<dg::IPipelineState>& pipelineState)
    : m_pipelineState(pipelineState) {

}

void Material::Bind() {

}
