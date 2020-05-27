#pragma once

#include <memory>

#include "core/common/ctor.h"
#include "middleware/gscheme/rttr/type.h"
#include "middleware/gscheme/rttr/variant.h"


class GSType : Fixed {
public:
    GSType() = delete;
    GSType(const rttr::variant& instance, const rttr::property& property);
    virtual ~GSType() = default;

    const char* GetName() const;

    virtual void DrawEditGui() = 0;

protected:
    const rttr::variant m_instance;
    const rttr::property m_property;
};

class GSTypeFloat : public GSType {
public:
    GSTypeFloat() = delete;
    GSTypeFloat(const rttr::variant& instance, const rttr::property& property);
    virtual ~GSTypeFloat() = default;

    void DrawEditGui() override;
};

std::shared_ptr<GSType> CreateGSType(const rttr::variant& instance, const rttr::property& property);
