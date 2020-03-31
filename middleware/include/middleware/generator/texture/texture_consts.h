#pragma once

#include <cstdint>

const uint32_t TextureGenGroupID = 1;

enum class TexturePinType : uint32_t {
    None = 0,
    Noise3D = (TextureGenGroupID << 16) + 0,
    Noise2D = (TextureGenGroupID << 16) + 1,
    Texture = (TextureGenGroupID << 16) + 2,
};
