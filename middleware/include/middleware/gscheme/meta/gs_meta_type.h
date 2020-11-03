#pragma once

#include <string>
#include <functional>

#include "core/common/ctor.h"


namespace cpgf {
    class GVariant;
}

namespace gs {

class MetaType : Fixed {
public:
    using FuncToString = std::function<std::string (const cpgf::GVariant&)>;
    using FuncFromString = std::function<bool (const std::string&, cpgf::GVariant&)>;

public:
    MetaType() = delete;
    MetaType(FuncToString&& funcToString, FuncFromString&& funcFromString);

public:
    const FuncToString toString;
    const FuncFromString fromString;
};

}
