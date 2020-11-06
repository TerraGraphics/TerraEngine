#include "middleware/gscheme/meta/gs_meta_type.h"

#include <vector>
#include <string>
#include <utility>

#include "core/common/exception.h"


namespace gs {

struct FieldByIndex {
    size_t index;
    std::string name;
    MetaType* metaType;
};

struct MetaType::Impl : Fixed {
    Impl() = default;
    Impl(FuncToString&& toString, FuncFromString&& fromString);
    ~Impl() = default;

    const FuncToString m_toString;
    const FuncFromString m_fromString;

    std::vector<FieldByIndex> m_fields;
};

MetaType::Impl::Impl(FuncToString&& toString, FuncFromString&& fromString)
    : m_toString(std::move(toString))
    , m_fromString(std::move(fromString)) {

}

MetaType::MetaType()
    : impl() {

}

MetaType::MetaType(FuncToString&& toString, FuncFromString&& fromString)
    : impl(std::move(toString), std::move(fromString)) {

}

MetaType::~MetaType() {

}

void MetaType::AddFieldByIndex(size_t index, std::string_view name, MetaType* metaType) {
    for (const auto& field: impl->m_fields) {
        if (field.index == index) {
            throw EngineError("gs::MetaType::AddFieldByIndex: index = {} with name = {} already exists", index, name);
        }
        if (field.name == name) {
            throw EngineError("gs::MetaType::AddFieldByIndex: name = {} with index = {} already exists", name, index);
        }
    }
    impl->m_fields.push_back(FieldByIndex{index, std::string(name), metaType});
}

}
