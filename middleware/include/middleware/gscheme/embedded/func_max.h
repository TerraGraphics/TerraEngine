#pragma once

#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/embedded/func_base.h"


namespace gs {

class FuncMax : public FuncBinaryUniversal {
public:
    FuncMax() = default;
    ~FuncMax() = default;

    UniversalType Result() const;
};

}
