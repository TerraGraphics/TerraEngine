#pragma once

#include "eigen/core.h"
#include "core/common/ctor.h"


namespace gs {

class FuncConstant : Fixed {
public:
    FuncConstant() = default;
    ~FuncConstant() = default;

    float Result() const { return m_value; }

    float GetValue() const { return m_value; }
    void SetValue(const float v);

private:
    float m_value = 0.f;
};

class FuncConstant2 : Fixed {
public:
    FuncConstant2() = default;
    ~FuncConstant2() = default;

    Eigen::Vector2f Result() const { return m_value; }

    Eigen::Vector2f GetValue() const { return m_value; }
    void SetValue(const Eigen::Vector2f v);

private:
    Eigen::Vector2f m_value = Eigen::Vector2f::Zero();
};

class FuncConstant3 : Fixed {
public:
    FuncConstant3() = default;
    ~FuncConstant3() = default;

    Eigen::Vector3f Result() const { return m_value; }

    Eigen::Vector3f GetValue() const { return m_value; }
    void SetValue(const Eigen::Vector3f v);

private:
    Eigen::Vector3f m_value = Eigen::Vector3f::Zero();
};

class FuncConstant4 : Fixed {
public:
    FuncConstant4() = default;
    ~FuncConstant4() = default;

    Eigen::Vector4f Result() const { return m_value; }

    Eigen::Vector4f GetValue() const { return m_value; }
    void SetValue(const Eigen::Vector4f v);

private:
    Eigen::Vector4f m_value = Eigen::Vector4f::Zero();
};



}
