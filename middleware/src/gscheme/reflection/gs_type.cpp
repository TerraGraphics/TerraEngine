#include "middleware/gscheme/reflection/gs_type.h"

#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"


GSType::GSType(const rttr::variant& instance, const rttr::property& property)
    : m_instance(instance)
    , m_property(property) {

}

const char* GSType::GetName() const {
    return m_property.get_name().empty() ? "<name>" : m_property.get_name().cbegin();
}

GSTypeFloat::GSTypeFloat(const rttr::variant& instance, const rttr::property& property)
    : GSType(instance, property) {

}

void GSTypeFloat::DrawEditGui() {
    float value = m_property.get_value(m_instance).get_value<float>();
    if (gui::InputScalar<float>(GetName(), value, 0.0001f, "{:.4f}")) {
        m_property.set_value(m_instance, value);
    }
}

std::shared_ptr<GSType> CreateGSType(const rttr::variant& instance, const rttr::property& property) {
    auto gsType = property.get_type();
    if (gsType == rttr::type::get<float>()) {
        return std::make_shared<GSTypeFloat>(instance, property);
    }

    throw EngineError("CreateGSType: Unknown type {}", gsType.get_name().to_string());
}
