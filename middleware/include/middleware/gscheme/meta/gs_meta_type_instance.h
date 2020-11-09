#pragma once


#include "core/common/ctor.h"
#include "middleware/gscheme/meta/gs_meta_type_interface.h"


namespace gs {

class MetaTypeInstance : Fixed {
public:
    MetaTypeInstance() = delete;
    MetaTypeInstance(IMetaPrimitiveTypeEdit* primitiveType);
    MetaTypeInstance(IMetaCompositeType* compositeType);
    ~MetaTypeInstance();

    bool IsChanged() const;
    bool IsPrimitiveType() const;

    void Init(const cpgf::GVariant& value);
    cpgf::GVariant Result() const;

    size_t Count() const;
    std::string_view GetName(size_t index) const;
    IMetaPrimitiveType* GetValue(size_t index) const;

protected:
    IMetaPrimitiveTypeEdit* m_primitiveType = nullptr;
    IMetaCompositeType* m_compositeType = nullptr;
};

class MetaTypeInstanceEdit : public MetaTypeInstance {
public:
    MetaTypeInstanceEdit(IMetaPrimitiveTypeEdit* primitiveType);
    MetaTypeInstanceEdit(IMetaCompositeType* compositeType);

    bool IsChanged() const;

    void Init(const cpgf::GVariant& value);
    cpgf::GVariant Result() const;
};

}
