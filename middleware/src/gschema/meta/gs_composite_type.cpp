#include "middleware/gschema/meta/gs_composite_type.h"

#include <memory>

#include "core/common/exception.h"
#include "middleware/gschema/meta/gs_type_interface.h"


namespace gs {

CompositeType::CompositeType(std::type_index typeIndex, const std::vector<CompositeTypeItem>& properties)
    : m_typeIndex(typeIndex)
    , m_properties(properties) {

}

CompositeType::~CompositeType() {
    for(auto& property: m_properties) {
        delete property.primitiveType;
    }
}

void CompositeType::HideInGUI() noexcept {
    m_visibleInGUI = false;
}

bool CompositeType::IsVisibleInGUI() const noexcept {
    return m_visibleInGUI;
}

std::type_index CompositeType::GetTypeIndex() const noexcept {
    return m_typeIndex;
}

bool CompositeType::IsChanged() const {
    for(const auto& property: m_properties) {
        if (property.primitiveType->IsChanged()) {
            return true;
        }
    }

    return false;
}

size_t CompositeType::CountItem() const {
    return m_properties.size();
}

IPrimitiveType* CompositeType::GetItemValue(size_t index) const {
    if (index >= m_properties.size()) {
        throw EngineError("gs::CompositeType::GetItemValue: index {} is not valid", index);
    }

    return static_cast<IPrimitiveType*>(m_properties[index].primitiveType);
}

}
