#pragma once

#include "core/common/ctor.h"
#include "middleware/gscheme/graph/gs_types.h"


namespace gs {

class FuncAdd : Fixed {
public:
    FuncAdd() = default;
    ~FuncAdd() = default;

    UniversalType Result() const;

    UniversalType GetA() const { return m_a; }
    void SetA(const UniversalType v) { m_a = v; }
    UniversalType GetB() const { return m_b; }
    void SetB(const UniversalType v) { m_b = v; }

private:
    UniversalType m_a = 0.f;
    UniversalType m_b = 0.f;
};

class TypesConvertStorage;
class TypeAdd : Fixed {
public:
    TypeAdd() = delete;
    TypeAdd(const TypesConvertStorage* typesConvertStorage);
    ~TypeAdd() = default;

    TypeId Result() const;

    TypeId GetA() const { return m_a; }
    void SetA(const TypeId v) { m_a = v; }
    TypeId GetB() const { return m_b; }
    void SetB(const TypeId v) { m_b = v; }

    bool IsValid();
    void Apply();
    void Reset();

private:
    TypeId m_a = TypeId::Float;
    TypeId m_originA = TypeId::Float;
    TypeId m_b = TypeId::Float;
    TypeId m_originB = TypeId::Float;
    const TypesConvertStorage* m_typesConvertStorage;
};

}
