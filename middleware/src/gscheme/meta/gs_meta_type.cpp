#include "middleware/gscheme/meta/gs_meta_type.h"

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

void MetaEnum::AddField(ptrdiff_t value, std::string_view name) {
    for (const auto& field: m_fields) {
        if (field.value == value) {
            throw EngineError("gs::MetaType::MetaEnum: value = {} with name = {} already exists", value, name);
        }
        if (field.name == name) {
            throw EngineError("gs::MetaType::MetaEnum: name = {} with value = {} already exists", name, value);
        }
    }
    m_fields.push_back(Field{value, std::string(name)});
}

}
