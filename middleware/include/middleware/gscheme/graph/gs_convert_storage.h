#pragma once

#include "core/common/ctor.h"
#include "core/common/pimpl.h"
#include "middleware/gscheme/graph/gs_types_decl.h"


namespace gs {

class ConvertStorage : Fixed {
public:
    ConvertStorage();
    ~ConvertStorage();

    bool CanConvert(TypeId from, TypeId to);

private:
    struct Impl;
    Pimpl<Impl, 56, 8> impl;
};

}
