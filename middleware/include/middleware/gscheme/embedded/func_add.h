#pragma once

#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/embedded/func_base.h"


namespace gs {

class FuncAdd : public FuncBinaryUniversal {
public:
    FuncAdd() = default;
    ~FuncAdd() = default;

    UniversalType Result() const;
};

}
