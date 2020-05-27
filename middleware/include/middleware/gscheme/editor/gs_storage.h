#pragma once

#include <string>

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
    void Draw();
    void DrawProperty();

private:
    struct Impl;
    Pimpl<Impl, 144, 8> impl;
};
