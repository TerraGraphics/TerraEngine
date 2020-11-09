#pragma once

#include <string>
#include <string_view>


namespace cpgf {
    class GVariant;
}

namespace gs {

struct IMetaPrimitiveType {
    virtual ~IMetaPrimitiveType() = default;

    virtual std::string ToString() const = 0;
    virtual bool FromString(const std::string& value) = 0;
};

struct IMetaPrimitiveTypeEdit : IMetaPrimitiveType {
    ~IMetaPrimitiveTypeEdit() override = default;

    virtual bool IsChanged() const = 0;
    virtual void SetValue(const cpgf::GVariant& value) = 0;
    virtual cpgf::GVariant GetValue() const = 0;
};

struct IMetaCompositeType {
    virtual ~IMetaCompositeType() = default;

    virtual bool IsChanged() const = 0;
    virtual void SetValue(const cpgf::GVariant& value) = 0;
    virtual cpgf::GVariant GetValue() const = 0;

    virtual size_t CountItem() const = 0;
    virtual std::string_view GetItemName(size_t index) const = 0;
    virtual IMetaPrimitiveType* GetItemValue(size_t index) const = 0;
};

}
