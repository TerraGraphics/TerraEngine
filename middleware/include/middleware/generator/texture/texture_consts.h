#pragma once

#include <cstdint>

const uint32_t TextureGenGroupID = 1;

enum class TextureGenTypeIDs : uint32_t {
    Noise3D = (TextureGenGroupID << 16) + 0,
    Noise2D = (TextureGenGroupID << 16) + 1,
    NoiseRasterization2D = (TextureGenGroupID << 16) + 2,
};
