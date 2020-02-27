#pragma once

#include <vector>
#include <string>
#include <cstddef>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class StaticVarsStorage : Fixed {
    struct Info {
        dg::SHADER_TYPE shaderType;
        std::string name;
    };
public:
    StaticVarsStorage() = delete;
    StaticVarsStorage(const DevicePtr& device, const ContextPtr& context);
    ~StaticVarsStorage();

    uint32_t Add(dg::SHADER_TYPE shaderType, const std::string& name, size_t size);
    void Update(uint32_t id, const void* data, size_t size);

    void SetVars(PipelineStateRawPtr pipelineState);

private:
    DevicePtr m_device;
    ContextPtr m_context;
    std::vector<Info> m_infoList;
    std::vector<BufferRawPtr> m_buffers;
};
