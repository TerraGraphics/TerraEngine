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
    const std::vector<FieldByIndex>& GetFields() const { return m_fields; }

private:
    std::vector<FieldByIndex> m_fields;
};

class MetaEnum : Fixed {
public:
    struct Field {
        ptrdiff_t value;
        std::string name;
    };

public:
    MetaEnum() = default;
    ~MetaEnum() = default;

public:
    void AddField(ptrdiff_t value, std::string_view name);
    const std::vector<Field>& GetFields() const { return m_fields; }

private:
    std::vector<Field> m_fields;
};

}
