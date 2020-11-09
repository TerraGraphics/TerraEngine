#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <typeindex>
#include <string_view>

#include "core/common/ctor.h"


namespace gs {

class MetaType : Fixed {
public:
    struct FieldByIndex {
        ptrdiff_t index;
        std::string name;
        std::type_index id;
    };

public:
    MetaType() = default;
    ~MetaType() = default;

    void AddFieldByIndex(ptrdiff_t index, std::string_view name, std::type_index id);

private:
    std::vector<FieldByIndex> m_fields;
};

}
