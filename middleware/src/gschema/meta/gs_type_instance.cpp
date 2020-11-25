#include "middleware/gschema/meta/gs_type_instance.h"

#include "cpgf/variant.h"
#include "core/common/exception.h"
#include "middleware/gschema/meta/gs_type_interface.h"
#include "middleware/gschema/meta/gs_composite_type.h"


namespace gs {

TypeInstance::TypeInstance(IPrimitiveTypeEdit* primitiveType, CompositeType* compositeType)
    : m_primitiveType(primitiveType)
    , m_compositeType(compositeType) {

}

TypeInstance::~TypeInstance() {
    if (IsPrimitiveType()) {
        delete m_primitiveType;
    } else {
        delete m_compositeType;
    }
}

bool TypeInstance::IsPrimitiveType() const {
    return (m_primitiveType != nullptr);
}

bool TypeInstance::IsEnabledUI() const {
    if (IsPrimitiveType()) {
        return m_primitiveType->IsEnabledUI();
    }

    return m_compositeType->IsEnabledUI();
}

std::type_index TypeInstance::GetTypeIndex() const {
    if (IsPrimitiveType()) {
        return m_primitiveType->GetTypeIndex();
    }

    return m_compositeType->GetTypeIndex();
}

size_t TypeInstance::Count() const {
    if (IsPrimitiveType()) {
        return 1;
    }

    return m_compositeType->CountItem();
}

IPrimitiveType* TypeInstance::GetValue(size_t index) const {
    if (IsPrimitiveType()) {
        if (index != 0) {
            throw EngineError("gs::TypeInstance::GetValue: no field with index higher than 0 is defined for primitive type");
        }
        return static_cast<IPrimitiveType*>(m_primitiveType);
    }

    return m_compositeType->GetItemValue(index);
}

TypeInstanceEdit::TypeInstanceEdit(IPrimitiveTypeEdit* primitiveType)
    : TypeInstance(primitiveType, nullptr) {

}

TypeInstanceEdit::TypeInstanceEdit(CompositeType* compositeType)
    : TypeInstance(nullptr, compositeType) {

}

bool TypeInstanceEdit::IsChanged() const {
    if (IsPrimitiveType()) {
        return m_primitiveType->IsChanged();
    }

    return m_compositeType->IsChanged();
}

void TypeInstanceEdit::Init(const cpgf::GVariant& value) {
    if (IsPrimitiveType()) {
        m_primitiveType->SetValue(value);
    } else {
        m_compositeType->SetValue(value);
    }
}

cpgf::GVariant TypeInstanceEdit::Result() const {
    if (IsPrimitiveType()) {
        return m_primitiveType->GetValue();
    }

    return m_compositeType->GetValue();
}

}
