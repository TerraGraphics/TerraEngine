#include "middleware/gschema/meta/gs_meta_type.h"

#include <vector>
#include <string>

#include "core/common/exception.h"


namespace gs {

void MetaType::AddFieldByIndex(ptrdiff_t index, std::string_view name, std::type_index id) {
    for (const auto& field: m_fields) {
        if (field.index == index) {
            throw EngineError("gs::MetaType::AddFieldByIndex: index = {} with name = {} already exists", index, name);
        }
        if (field.name == name) {
            throw EngineError("gs::MetaType::AddFieldByIndex: name = {} with index = {} already exists", name, index);
        }
    }
    m_fields.push_back(Field{index, std::string(name), id});
}

void MetaEnum::AddField(ValueType value, std::string_view name, std::string_view displayName) {
    for (const auto& field: m_fields) {
        if (field.value == value) {
            throw EngineError("gs::MetaType::MetaEnum: value = {} with name = {} already exists", value, name);
        }
        if (field.name == name) {
            throw EngineError("gs::MetaType::MetaEnum: name = {} with value = {} already exists", name, value);
        }
        if (!displayName.empty() && (field.displayName == displayName)) {
            throw EngineError("gs::MetaType::MetaEnum: displayName = {} with value = {} already exists", displayName, value);
        }
    }

    if (displayName.empty()) {
        m_fields.push_back(Field{value, std::string(name), std::string(name)});
        m_displayNames.push_back(std::string(name));
    } else {
        m_fields.push_back(Field{value, std::string(name), std::string(displayName)});
        m_displayNames.push_back(std::string(displayName));
    }
}

std::string MetaEnum::GetDisplayNameByValue(ValueType value) const {
    for (const auto& field: m_fields) {
        if (field.value == value) {
            return field.displayName;
        }
    }

    throw EngineError("gs::MetaType::GetDisplayNameByValue: value = {} not exists", value);
}

MetaEnum::ValueType MetaEnum::GetValueByDisplayName(const std::string& displayName) const {
    for (const auto& field: m_fields) {
        if (field.displayName == displayName) {
            return field.value;
        }
    }

    throw EngineError("gs::MetaType::GetValueByDisplayName: displayName = {} not exists", displayName);
}

}
