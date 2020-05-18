#pragma once

#include <string>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "core/material/material.h"
#include "middleware/std_render/structures.h"


namespace Diligent {
    struct SamplerDesc;
}
class MaterialView;
class StdMaterial : public Material {
public:
    StdMaterial() = delete;
    StdMaterial(const std::string& name);
    ~StdMaterial() override;

    void Depth(bool enable) noexcept;
    void SetCullMode(dg::CULL_MODE value) noexcept;
    void SetTopology(dg::PRIMITIVE_TOPOLOGY value) noexcept;

    void SetBaseColor(const math::Color4f& value);
    void SetBaseColor(const dg::float4& value);
    void SetBaseColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);

    const dg::SamplerDesc& GetBaseTextureDesc();
    void SetBaseTextureDesc(const dg::SamplerDesc& desc);
    void SetBaseTextureAddressMode(dg::TEXTURE_ADDRESS_MODE mode);
    void SetBaseTexture(TextureViewPtr& texture);

    void SetAlphaThreshold(float value);

    void AmbientDiffuse(bool enable);
    void Specular(bool enable);
    void ColorPicker(bool enable);

protected:
    void OnNewFrame() override;
    void OnNewView(MaterialView& view) override;

    void ApplyMask(uint64_t mask);
    void AddFlag(uint64_t flag);
    void RemoveFlag(uint64_t flag);
    bool AddAndRemoveFlag(uint64_t addFlag, uint64_t removeFlag);

private:
    bool m_dataEnable = false;
    uint16_t m_materialVarId = 0;
    uint16_t m_baseTextureVarId = 0;
    TextureViewPtr m_baseTexture;
    dg::ShaderMaterial m_data;
    BufferPtr m_buffer;
};

class StdMaterialGrass : public StdMaterial {
public:
    StdMaterialGrass() = delete;
    StdMaterialGrass(const std::string& name);
    ~StdMaterialGrass() override;
};
