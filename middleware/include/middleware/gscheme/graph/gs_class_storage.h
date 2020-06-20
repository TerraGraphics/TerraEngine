#pragma once

#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace gs {

class Class;
class ClassStorage : Fixed {
public:
    ClassStorage();
    ~ClassStorage();

    Class* GetClass(uint16_t classIndex);
    Class* GetClass(std::string_view name);
    uint16_t GetClassIndex(std::string_view name);

    const Class* ClassesBegin() const noexcept;
    const Class* ClassesEnd() const noexcept;

private:
    struct Impl;
    Pimpl<Impl, 128, 8> impl;
};

}
