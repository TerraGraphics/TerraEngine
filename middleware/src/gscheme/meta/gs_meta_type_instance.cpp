#include "middleware/gscheme/meta/gs_meta_type_instance.h"

#include "core/common/exception.h"


namespace gs {

MetaTypeInstance::MetaTypeInstance(IMetaPrimitiveType* primitiveType)
    : m_primitiveType(primitiveType) {

}

MetaTypeInstance::MetaTypeInstance(IMetaCompositeType* compositeType)
    : m_compositeType(compositeType) {

}

bool MetaTypeInstance::IsPrimitiveType() const {
    return (m_primitiveType != nullptr);
}

size_t MetaTypeInstance::Count() const {
    if (IsPrimitiveType()) {
        return 1;
    }

    return m_compositeType->Count();
}

std::string_view MetaTypeInstance::GetName(size_t index) const {
    if (IsPrimitiveType()) {
        throw EngineError("gs::MetaTypeInstance::GetName: no field name specified for primitive type");
    }

    return m_compositeType->GetName(index);
}

IMetaPrimitiveType* MetaTypeInstance::GetValue(size_t index) const {
    if (IsPrimitiveType()) {
        if (index != 0) {
            throw EngineError("gs::MetaTypeInstance::GetName: no field with index higher than 0 is defined for primitive type");
        }
        return m_primitiveType;
    }

    return m_compositeType->GetValue(index);
}

}
