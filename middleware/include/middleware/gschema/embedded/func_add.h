#pragma once

#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/embedded/func_base.h"


namespace gs {

class FuncAdd : public FuncBinaryUniversal {
public:
    FuncAdd() = default;
    ~FuncAdd() = default;

    UniversalType Result() const;
};

}
