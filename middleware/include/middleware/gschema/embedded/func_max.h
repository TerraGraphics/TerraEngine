#pragma once

#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/embedded/func_base.h"


namespace gs {

class FuncMax : public FuncBinaryUniversal {
public:
    FuncMax() = default;
    ~FuncMax() = default;

    UniversalType Result() const;
};

}
