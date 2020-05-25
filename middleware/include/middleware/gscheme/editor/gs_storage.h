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

private:
    struct Impl;
    Pimpl<Impl, 136, 8> impl;
};
