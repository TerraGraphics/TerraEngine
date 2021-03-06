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

    virtual bool IsEnumType() const noexcept = 0;
    virtual bool IsIntegerType() const noexcept = 0;
    virtual bool IsFloatingType() const noexcept = 0;
    virtual bool IsVisibleInGUI() const noexcept = 0;
    virtual bool IsShowStepButtons() const noexcept = 0;
    virtual std::string_view GetDisplayName() const = 0;
    virtual const std::vector<std::string>& GetItems() const = 0;

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
