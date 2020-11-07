#pragma once

#include <string>
#include <string_view>


namespace gs {

class IMetaPrimitiveType {
public:
    virtual ~IMetaPrimitiveType() = default;

    virtual std::string ToString() const = 0;
    virtual bool FromString(const std::string& value) = 0;
};

class IMetaCompositeType {
public:
    virtual size_t Count() const = 0;
    virtual std::string_view GetName(size_t index) const = 0;
    virtual IMetaPrimitiveType* GetValue(size_t index) const = 0;
};

}
