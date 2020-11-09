#pragma once


#include "core/common/ctor.h"
#include "middleware/gscheme/meta/gs_type_interface.h"


namespace gs {

class TypeInstance : Fixed {
public:
    TypeInstance() = delete;
    TypeInstance(IPrimitiveTypeEdit* primitiveType);
    TypeInstance(ICompositeType* compositeType);
    ~TypeInstance();

    bool IsChanged() const;
    bool IsPrimitiveType() const;

    void Init(const cpgf::GVariant& value);
    cpgf::GVariant Result() const;

    size_t Count() const;
    std::string_view GetName(size_t index) const;
    IPrimitiveType* GetValue(size_t index) const;

protected:
    IPrimitiveTypeEdit* m_primitiveType = nullptr;
    ICompositeType* m_compositeType = nullptr;
};

class TypeInstanceEdit : public TypeInstance {
public:
    TypeInstanceEdit(IPrimitiveTypeEdit* primitiveType);
    TypeInstanceEdit(ICompositeType* compositeType);

    bool IsChanged() const;

    void Init(const cpgf::GVariant& value);
    cpgf::GVariant Result() const;
};

}
