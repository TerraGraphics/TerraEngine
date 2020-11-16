#pragma once

#include <string>
#include <typeindex>
#include <string_view>


namespace cpgf {
    class GVariant;
}

namespace gs {

struct IPrimitiveType {
    virtual ~IPrimitiveType() = default;

    virtual bool IsIntegerType() const noexcept = 0;
    virtual bool IsFloatingType() const noexcept = 0;
    virtual bool IsEnabledUI() const noexcept = 0;
    virtual bool IsEnabledShowStepButtons() const noexcept = 0;
    virtual std::string_view GetPrettyName() const = 0;

    virtual void Inc() = 0;
    virtual void Dec() = 0;
    virtual std::string ToString() const = 0;
    virtual void FromString(const std::string& value) = 0;
};

struct IPrimitiveTypeEdit : IPrimitiveType {
    ~IPrimitiveTypeEdit() override = default;

    virtual std::type_index GetTypeIndex() const = 0;

    virtual bool IsChanged() const = 0;
    virtual void SetValue(const cpgf::GVariant& value) = 0;
    virtual cpgf::GVariant GetValue() const = 0;
};

}
