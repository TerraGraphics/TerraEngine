#pragma once

#include <string>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace rttr {
    class variant;
}

class GSNode : Fixed {
public:
    GSNode() = delete;
    GSNode(const std::string& name, rttr::variant&& instance);
    ~GSNode();

    void Draw(uint8_t alpha, TextureViewRaw texBackground, float texWidth, float texHeight);

private:
    struct Impl;
    Pimpl<Impl, 48, 8> impl;
};
