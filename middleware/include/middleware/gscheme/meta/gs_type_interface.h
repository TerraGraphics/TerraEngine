#pragma once

#include <string>
#include <string_view>


namespace cpgf {
    class GVariant;
}

namespace gs {

struct IPrimitiveType {
    virtual ~IPrimitiveType() = default;

    virtual std::string ToString() const = 0;
    virtual bool FromString(const std::string& value) = 0;
};

struct IPrimitiveTypeEdit : IPrimitiveType {
    ~IPrimitiveTypeEdit() override = default;

    virtual bool IsChanged() const = 0;
    virtual void SetValue(const cpgf::GVariant& value) = 0;
    virtual cpgf::GVariant GetValue() const = 0;
};

struct ICompositeType {
    virtual ~ICompositeType() = default;

    virtual bool IsChanged() const = 0;
    virtual void SetValue(const cpgf::GVariant& value) = 0;
    virtual cpgf::GVariant GetValue() const = 0;

    virtual size_t CountItem() const = 0;
    virtual std::string_view GetItemName(size_t index) const = 0;
    virtual IPrimitiveType* GetItemValue(size_t index) const = 0;
};

}