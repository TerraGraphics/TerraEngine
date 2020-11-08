#include "middleware/gscheme/meta/gs_meta_type.h"

#include <vector>
#include <string>
#include <utility>

#include "core/common/exception.h"


namespace gs {

MetaType::MetaType() {

}

MetaType::~MetaType() {

}

void MetaType::AddFieldByIndex(size_t index, std::string_view name, std::type_index id) {
    for (const auto& field: m_fields) {
        if (field.index == index) {
            throw EngineError("gs::MetaType::AddFieldByIndex: index = {} with name = {} already exists", index, name);
        }
        if (field.name == name) {
            throw EngineError("gs::MetaType::AddFieldByIndex: name = {} with index = {} already exists", name, index);
        }
    }
    m_fields.push_back(FieldByIndex{index, std::string(name), id});
}

}
