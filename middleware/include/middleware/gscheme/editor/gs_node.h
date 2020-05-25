#pragma once

#include <cstdint>
#include <string_view>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace rttr {
    class variant;
}

class GSNode : Fixed {
public:
    GSNode() = delete;
    GSNode(uintptr_t id, std::string_view name, rttr::variant&& instance);
    ~GSNode();

    void Draw(uint8_t alpha, TextureViewRaw texBackground, float texWidth, float texHeight);

private:
    struct Impl;
    Pimpl<Impl, 40, 8> impl;
};
