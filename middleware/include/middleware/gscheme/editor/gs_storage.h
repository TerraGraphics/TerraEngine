#pragma once

#include <string>
#include <cstdint>

#include "core/dg/dg.h"
#include "core/common/ctor.h"
#include "core/common/pimpl.h"


class GSStorage : Fixed {
public:
    GSStorage() = delete;
    GSStorage(TexturePtr& texBackground);
    ~GSStorage();

    void Create();

    bool AddNode(const std::string& name);
    bool AddLink(uintptr_t pinFirst, uintptr_t pinSecond, bool checkOnly);

    void Draw();
    void DrawProperty();

private:
    struct Impl;
    Pimpl<Impl, 256, 8> impl;
};
