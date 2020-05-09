#pragma once

#include <string>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/math/types.h"
#include "core/material/material_new.h"
#include "middleware/std_material/structures.h"


class MaterialView;
class StdMaterialNew : public MaterialNew {
public:
    StdMaterialNew() = delete;
    StdMaterialNew(const std::string& name);
    ~StdMaterialNew() override;

    void SetBaseColor(const math::Color4f& value);
    void SetBaseColor(const dg::float4& value);
    void SetBaseColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    void SetBaseTexture(TextureViewPtr& texture);
    void SetAlphaThreshold(float value);

    void AmbientDiffuse(bool enable);
    void Specular(bool enable);

protected:
    void OnNewFrame() override;
    void OnNewView(MaterialView& view) override;

private:
    void FillMeta();
    void AddFlag(uint64_t flag);
    void RemoveFlag(uint64_t flag);
    bool AddAndRemoveFlag(uint64_t addFlag, uint64_t removeFlag);

private:
    TextureViewPtr m_baseTexture;
    dg::ShaderMaterial m_data;
    BufferPtr m_buffer;
};
