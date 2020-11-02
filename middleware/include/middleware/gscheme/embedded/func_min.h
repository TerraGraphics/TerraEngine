#pragma once

#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/embedded/func_base.h"


namespace gs {

class FuncMin : public FuncBinaryUniversal {
public:
    FuncMin() = default;
    ~FuncMin() = default;

    UniversalType Result() const;
};

}
