#include "middleware/gscheme/meta/gs_meta_type_instance.h"

#include "cpgf/variant.h"
#include "core/common/exception.h"


namespace gs {

MetaTypeInstance::MetaTypeInstance(IMetaPrimitiveTypeEdit* primitiveType)
    : m_primitiveType(primitiveType) {

}

MetaTypeInstance::MetaTypeInstance(IMetaCompositeType* compositeType)
    : m_compositeType(compositeType) {

}

bool MetaTypeInstance::IsChanged() const {
    if (IsPrimitiveType()) {
        return m_primitiveType->IsChanged();
    }

    return m_compositeType->IsChanged();
}

bool MetaTypeInstance::IsPrimitiveType() const {
    return (m_primitiveType != nullptr);
}

void MetaTypeInstance::Init(const cpgf::GVariant& value) {
    if (IsPrimitiveType()) {
        m_primitiveType->SetValue(value);
    } else {
        m_compositeType->SetValue(value);
    }
}

cpgf::GVariant MetaTypeInstance::Result() const {
    if (IsPrimitiveType()) {
        return m_primitiveType->GetValue();
    }

    return m_compositeType->GetValue();
}

size_t MetaTypeInstance::Count() const {
    if (IsPrimitiveType()) {
        return 1;
    }

    return m_compositeType->CountItem();
}

std::string_view MetaTypeInstance::GetName(size_t index) const {
    if (IsPrimitiveType()) {
        throw EngineError("gs::MetaTypeInstance::GetName: no field name specified for primitive type");
    }

    return m_compositeType->GetItemName(index);
}

IMetaPrimitiveType* MetaTypeInstance::GetValue(size_t index) const {
    if (IsPrimitiveType()) {
        if (index != 0) {
            throw EngineError("gs::MetaTypeInstance::GetName: no field with index higher than 0 is defined for primitive type");
        }
        return static_cast<IMetaPrimitiveType*>(m_primitiveType);
    }

    return m_compositeType->GetItemValue(index);
}

}
