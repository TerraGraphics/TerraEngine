#pragma once

#include "core/common/ctor.h"
#include "core/common/pimpl.h"
#include "middleware/gscheme/graph/gs_types_decl.h"
#include "middleware/gscheme/graph/gs_types_convert_func.h"


namespace gs {

// Basic type -> Basic type
// Basic type -> Universal type
// Concrete universal type -> Basic type
class TypesConvertStorage : Fixed {
public:
    TypesConvertStorage();
    ~TypesConvertStorage();

    bool CanConvert(TypeId from, TypeId to) const;
    ConvertFunc GetConvertFunc(TypeId from, TypeId to) const;

private:
    struct Impl;
    Pimpl<Impl, 56, 8> impl;
};

}
