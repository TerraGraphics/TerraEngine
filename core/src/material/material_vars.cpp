#include "core/material/material_vars.h"

#include <cstring>
#include <algorithm>

#include <DiligentCore/Graphics/GraphicsEngine/interface/ShaderResourceVariable.h>

#include "core/dg/dg.h"
#include "core/dg/device.h"
#include "core/dg/buffer.h"
#include "core/dg/context.h"
#include "core/common/exception.h"
#include "core/dg/graphics_types.h"
#include "core/dg/graphics_accessories.h"


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

uint32_t StaticVarsStorage::Add(dg::SHADER_TYPE shaderType, const std::string& name, size_t size) {
    for (const auto& it: m_infoList) {
        if ((it.shaderType == shaderType) && (it.name == name)) {
            throw EngineError("shader varaible with name {} is duplicated for shader type {} in StaticVarsStorage", name, dg::GetShaderTypeLiteralName(shaderType));
        }
    }

    BufferRaw buffer = nullptr;
    dg::BufferDesc desc;
    desc.Name = name.c_str();
    desc.uiSizeInBytes = static_cast<uint32_t>(size);
    desc.Usage = dg::USAGE_DYNAMIC;
    desc.BindFlags = dg::BIND_UNIFORM_BUFFER;
    desc.CPUAccessFlags = dg::CPU_ACCESS_WRITE;
    m_device->CreateBuffer(desc, nullptr, &buffer);

    auto id = static_cast<uint32_t>(m_infoList.size());
    m_buffers.push_back(buffer);
    m_infoList.push_back(Info{shaderType, name});

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

void StaticVarsStorage::SetVars(PipelineStateRaw pipelineState) {
    uint32_t ind = 0;
    for (const auto& it: m_infoList) {
        ShaderResourceVariableRaw var = pipelineState->GetStaticVariableByName(it.shaderType, it.name.c_str());
        if (var != nullptr) {
            var->Set(m_buffers[ind]);
        }
    }
}
