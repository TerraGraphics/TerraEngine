#pragma once

#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/embedded/func_base.h"


namespace gs {

class FuncMin : public FuncBinaryUniversal {
public:
    FuncMin() = default;
    ~FuncMin() = default;

    UniversalType Result() const;
};

}
