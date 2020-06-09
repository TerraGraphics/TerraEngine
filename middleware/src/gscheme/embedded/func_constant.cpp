#include "middleware/gscheme/embedded/func_constant.h"


namespace gs {

void FuncConstant::SetValue(const float v) noexcept {
    m_value = v;
}

void FuncConstant2::SetValue(const Eigen::Vector2f v) noexcept {
    m_value = v;
}

void FuncConstant3::SetValue(const Eigen::Vector3f v) noexcept {
    m_value = v;
}

void FuncConstant4::SetValue(const Eigen::Vector4f v) noexcept {
    m_value = v;
}

}
