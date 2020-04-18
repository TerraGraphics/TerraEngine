#pragma once

#include "core/dg/dg.h"
#include "core/math/types.h"


class INodePreview {
public:
    virtual TexturePtr GetTexture(math::Size size) = 0;
};
