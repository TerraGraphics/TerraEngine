#include "middleware/gscheme/meta/gs_type_instance.h"

#include "cpgf/variant.h"
#include "core/common/exception.h"


namespace gs {

TypeInstance::TypeInstance(IPrimitiveTypeEdit* primitiveType)
    : m_primitiveType(primitiveType) {

}

TypeInstance::TypeInstance(ICompositeType* compositeType)
    : m_compositeType(compositeType) {

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

size_t TypeInstance::Count() const {
    if (IsPrimitiveType()) {
        return 1;
    }

    return m_compositeType->CountItem();
}

std::string_view TypeInstance::GetName(size_t index) const {
    if (IsPrimitiveType()) {
        throw EngineError("gs::TypeInstance::GetName: no field name specified for primitive type");
    }

    return m_compositeType->GetItemName(index);
}

IPrimitiveType* TypeInstance::GetValue(size_t index) const {
    if (IsPrimitiveType()) {
        if (index != 0) {
            throw EngineError("gs::TypeInstance::GetName: no field with index higher than 0 is defined for primitive type");
        }
        return static_cast<IPrimitiveType*>(m_primitiveType);
    }

    return m_compositeType->GetItemValue(index);
}

TypeInstanceEdit::TypeInstanceEdit(IPrimitiveTypeEdit* primitiveType)
    : TypeInstance(primitiveType) {

}

TypeInstanceEdit::TypeInstanceEdit(ICompositeType* compositeType)
    : TypeInstance(compositeType) {

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
