#pragma once

#include <memory>
#include <vector>
#include <cstdint>
#include <string_view>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace rttr {
    class type;
    class variant;
}

class GSInputPin;
class GSOutputPin;
class GSNode : Fixed {
public:
    GSNode() = delete;
    GSNode(uintptr_t id, std::string_view name, const rttr::type& nodeType);
    ~GSNode();

    rttr::variant& GetInstance();

    void SetInputPins(std::vector<std::unique_ptr<GSInputPin>>&& pins);
    void GetOutputPins(std::vector<std::unique_ptr<GSOutputPin>>&& pins);

    void Draw(uint8_t alpha, TextureViewRaw texBackground, float texWidth, float texHeight);
    void DrawEditGui();

private:
    struct Impl;
    Pimpl<Impl, 96, 8> impl;
};
