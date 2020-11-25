#pragma once

#include "core/common/ctor.h"
#include "core/common/pimpl.h"
#include "middleware/gschema/graph/gs_types_decl.h"
#include "middleware/gschema/graph/gs_types_convert_func.h"


namespace gs {

// From type -> To type
// Basic -> Basic
// Basic -> Universal
// Concrete universal -> Basic
// Concrete universal -> Universal
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
