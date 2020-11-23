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
    using ValueType = uint64_t;
    struct Field {
        ValueType value;
        std::string name;
        std::string prettyName;
    };

public:
    MetaEnum() = default;
    ~MetaEnum() = default;

public:
    void AddField(ValueType value, std::string_view name, std::string_view prettyName);
    std::string GetPrettyNameByValue(ValueType value) const;
    ValueType GetValueByPrettyName(const std::string& prettyName) const;
    const std::vector<Field>& GetFields() const { return m_fields; }
    const std::vector<std::string>& GetPrettyNames() const { return m_prettyNames; }

private:
    std::vector<Field> m_fields;
    std::vector<std::string> m_prettyNames;
};

}
