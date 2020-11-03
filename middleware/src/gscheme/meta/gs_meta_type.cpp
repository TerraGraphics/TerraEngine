#include "middleware/gscheme/meta/gs_meta_type.h"


namespace gs {

MetaType::MetaType(FuncToString&& funcToString, FuncFromString&& funcFromString)
    : toString(std::move(funcToString))
    , fromString(std::move(funcFromString)) {

}

}
