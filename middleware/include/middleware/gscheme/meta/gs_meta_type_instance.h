#pragma once


#include "core/common/ctor.h"
#include "middleware/gscheme/meta/gs_meta_type_interface.h"


namespace gs {

class MetaTypeInstance : Fixed {
public:
    MetaTypeInstance() = delete;
    MetaTypeInstance(IMetaPrimitiveType* primitiveType);
    MetaTypeInstance(IMetaCompositeType* compositeType);
    ~MetaTypeInstance() = default;

    bool IsPrimitiveType() const;

    size_t Count() const;
    std::string_view GetName(size_t index) const;
    IMetaPrimitiveType* GetValue(size_t index) const;

private:
    IMetaPrimitiveType* m_primitiveType = nullptr;
    IMetaCompositeType* m_compositeType = nullptr;
};

}
