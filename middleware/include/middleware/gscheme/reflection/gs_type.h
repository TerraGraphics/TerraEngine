#pragma once

#include <memory>

#include "core/common/ctor.h"
#include "middleware/gscheme/rttr/type.h"


namespace rttr {
    class variant;
}

class GSType : Fixed {
public:
    GSType() = delete;
    GSType(rttr::variant& instance, const rttr::property& property);
    virtual ~GSType() = default;

    const char* GetName() const;
    bool IsEmbedded() const noexcept { return m_embedded; }

    virtual void DrawEditGui() = 0;

protected:
    const bool m_embedded;
    rttr::variant& m_instance;
    const rttr::property m_property;
};

class GSTypeFloat : public GSType {
public:
    GSTypeFloat() = delete;
    GSTypeFloat(rttr::variant& instance, const rttr::property& property);
    virtual ~GSTypeFloat() = default;

    void DrawEditGui() override;
};

std::shared_ptr<GSType> CreateGSType(rttr::variant& instance, const rttr::property& property);
