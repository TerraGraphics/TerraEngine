#pragma once

#include <vector>
#include <DiligentCore/Graphics/GraphicsEngine/interface/Shader.h>

#include "core/common/dg.h"
#include "core/common/ctor.h"


namespace Diligent {
    class IPipelineState;
}

class StaticVarsStorage : Fixed {
    struct Info {
        dg::SHADER_TYPE shaderType;
        std::string name;
    };
public:
    StaticVarsStorage() = delete;
    StaticVarsStorage(const DevicePtr& device, const ContextPtr& context);
    ~StaticVarsStorage();

    template<typename T> uint32_t Add(dg::SHADER_TYPE shaderType, const std::string& name, const T& data) {
        return Add(shaderType, name, reinterpret_cast<const void*>(&data), sizeof(T));
    }
    template<typename T> void Update(uint32_t id, const T& data) {
        Update(id, reinterpret_cast<const void*>(&data), sizeof(T));
    }

    uint32_t Add(dg::SHADER_TYPE shaderType, const std::string& name, const void* data, size_t size);
    void Update(uint32_t id, const void* data, size_t size);

    void SetVars(dg::IPipelineState* pipelineState);

private:
    DevicePtr m_device;
    ContextPtr m_context;
    std::vector<Info> m_infoList;
    std::vector<dg::IBuffer*> m_buffers;
};
