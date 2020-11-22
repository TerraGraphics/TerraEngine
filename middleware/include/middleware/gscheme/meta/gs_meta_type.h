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
    struct Field {
        ptrdiff_t index;
        std::string name;
        std::type_index id;
    };

public:
    MetaType() = default;
    ~MetaType() = default;

    void AddFieldByIndex(ptrdiff_t index, std::string_view name, std::type_index id);
    const std::vector<Field>& GetFields() const { return m_fields; }

private:
    std::vector<Field> m_fields;
};

class MetaEnum : Fixed {
public:
    struct Field {
        uint64_t value;
        std::string name;
        std::string prettyName;
    };

public:
    MetaEnum() = default;
    ~MetaEnum() = default;

public:
    void AddField(uint64_t value, std::string_view name, std::string_view prettyName);
    const std::vector<Field>& GetFields() const { return m_fields; }

private:
    std::vector<Field> m_fields;
};

}
