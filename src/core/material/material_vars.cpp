#include "core/material/material_vars.h"

#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>
#include <DiligentCore/Graphics/GraphicsAccessories/interface/GraphicsAccessories.h>

#include "core/common/exception.h"


StaticVarsStorage::StaticVarsStorage(const DevicePtr& device, const ContextPtr& context)
    : m_device(device)
    , m_context(context) {

}

StaticVarsStorage::~StaticVarsStorage() {
    for (auto* buffer: m_buffers) {
        buffer->Release();
    }
    m_buffers.clear();
    m_infoList.clear();
}

uint32_t StaticVarsStorage::Add(dg::SHADER_TYPE shaderType, const std::string& name, const void* data, size_t size) {
    for (const auto& it: m_infoList) {
        if ((it.shaderType == shaderType) && (it.name == name)) {
            throw EngineError("shader varaible with name {} is duplicated for shader type {} in StaticVarsStorage", name, dg::GetShaderTypeLiteralName(shaderType));
        }
    }

    dg::IBuffer* buffer;
    dg::CreateUniformBuffer(m_device, size, name.c_str(), &buffer);

    auto id = static_cast<uint32_t>(m_infoList.size());
    m_buffers.push_back(buffer);
    m_infoList.push_back(Info{shaderType, name});

    Update(id, data, size);
    return id;
}

void StaticVarsStorage::Update(uint32_t id, const void* data, size_t size) {
    if (m_buffers.size() <= static_cast<size_t>(id)) {
        throw EngineError("shader varaible with id {} not found in StaticVarsStorage", id);
    }

    void* dstData;
    auto* buffer = m_buffers[id];
    m_context->MapBuffer(buffer, dg::MAP_WRITE, dg::MAP_FLAG_DISCARD, dstData);
    memcpy(dstData, data, size);
    m_context->UnmapBuffer(buffer, dg::MAP_WRITE);
}

void StaticVarsStorage::SetVars(dg::IPipelineState* pipelineState) {
    uint32_t ind = 0;
    for (const auto& it: m_infoList) {
        dg::IShaderResourceVariable* var = pipelineState->GetStaticVariableByName(it.shaderType, it.name.c_str());
        if (var != nullptr) {
            var->Set(m_buffers[ind]);
        }
    }
}
