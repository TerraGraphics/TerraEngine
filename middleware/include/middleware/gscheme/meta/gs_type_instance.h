#pragma once

#include <typeindex>

#include "core/common/ctor.h"
#include "middleware/gscheme/meta/gs_type_interface.h"


namespace gs {

class CompositeType;
class TypeInstance : Fixed {
protected:
    TypeInstance() = delete;
    TypeInstance(IPrimitiveTypeEdit* primitiveType, CompositeType* compositeType);
    ~TypeInstance();

public:
    bool IsPrimitiveType() const;
    bool IsEnabledUI() const;
    std::type_index GetTypeIndex() const;

    size_t Count() const;
    std::string_view GetName(size_t index) const;
    IPrimitiveType* GetValue(size_t index) const;

protected:
    IPrimitiveTypeEdit* m_primitiveType = nullptr;
    CompositeType* m_compositeType = nullptr;
};

class TypeInstanceEdit : public TypeInstance {
public:
    TypeInstanceEdit(IPrimitiveTypeEdit* primitiveType);
    TypeInstanceEdit(CompositeType* compositeType);

    bool IsChanged() const;

    void Init(const cpgf::GVariant& value);
    cpgf::GVariant Result() const;
};

}
