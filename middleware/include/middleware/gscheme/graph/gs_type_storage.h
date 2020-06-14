#pragma once

#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace gs {

class TypeClass;
class TypeStorage : Fixed {
public:
    TypeStorage();
    ~TypeStorage();

    TypeClass* GetTypeClass(uint16_t typeClassIndex);
    TypeClass* GetTypeClass(std::string_view name);
    uint16_t GetTypeClassIndex(std::string_view name);

    const TypeClass* TypeClassesBegin() const noexcept;
    const TypeClass* TypeClassesEnd() const noexcept;

private:
    struct Impl;
    Pimpl<Impl, 128, 8> impl;
};

}
