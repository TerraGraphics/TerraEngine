#pragma once

#include <string>
#include <cstddef>
#include <functional>
#include <string_view>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace cpgf {
    class GVariant;
}

namespace gs {

class MetaType : Fixed {
public:
    using FuncToString = std::function<std::string (const cpgf::GVariant&)>;
    using FuncFromString = std::function<bool (const std::string&, cpgf::GVariant&)>;

public:
    MetaType();
    MetaType(FuncToString&& toString, FuncFromString&& fromString);
    ~MetaType();

    void AddFieldByIndex(size_t index, std::string_view name, MetaType* metaType);

private:
    struct Impl;
    Pimpl<Impl, 88, 8> impl;
};

}
