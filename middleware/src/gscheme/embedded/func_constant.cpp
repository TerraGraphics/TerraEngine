#include "middleware/gscheme/embedded/func_constant.h"


namespace gs {

void FuncConstant::SetValue(const float v) {
    m_value = v;
}

void FuncConstant2::SetValue(const Eigen::Vector2f v) {
    m_value = v;
}

void FuncConstant3::SetValue(const Eigen::Vector3f v) {
    m_value = v;
}

void FuncConstant4::SetValue(const Eigen::Vector4f v) {
    m_value = v;
}

}
