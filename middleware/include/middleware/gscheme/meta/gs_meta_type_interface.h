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

    virtual bool IsChached() const = 0;
    virtual void SetValue(const cpgf::GVariant& value) = 0;
    virtual cpgf::GVariant GetValue() const = 0;
};

struct IMetaCompositeType {
    virtual size_t Count() const = 0;
    virtual std::string_view GetName(size_t index) const = 0;
    virtual IMetaPrimitiveType* GetValue(size_t index) const = 0;
};

}
