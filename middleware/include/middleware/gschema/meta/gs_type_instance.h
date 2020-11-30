#pragma once

#include <cstddef>
#include <typeindex>

#include "core/common/ctor.h"


namespace cpgf {
    class GVariant;
}

namespace gs {

class CompositeType;
struct IPrimitiveType;
struct IPrimitiveTypeEdit;
class TypeInstance : Fixed {
protected:
    TypeInstance() = delete;
    TypeInstance(IPrimitiveTypeEdit* primitiveType, CompositeType* compositeType);
    ~TypeInstance();

public:
    bool IsPrimitiveType() const;
    bool IsVisibleInGUI() const;
    std::type_index GetTypeIndex() const;

    size_t Count() const;
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
