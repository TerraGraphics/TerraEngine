#include "middleware/gschema/meta/gs_define.h"

#include <string>
#include "core/common/exception.h"


namespace gs::detail {

DefineClass::DefineClass(cpgf::GMetaClass* metaClass)
    : m_metaClass(metaClass) {

}

DefineClass::DefineClass(cpgf::GMetaClass* metaClass, cpgf::GMetaConstructor* ctor, const char* displayName, bool isBaseClass) {
    m_metaClass = const_cast<cpgf::GMetaClass *>(cpgf::getGlobalMetaClass()->doGetClass(metaClass->getName().c_str()));
    if(m_metaClass == nullptr) {
        m_metaClass = metaClass;
        cpgf::getGlobalMetaClass()->addClass(m_metaClass);
    } else {
        delete metaClass;
    }

    if (!isBaseClass) {
        m_metaClass->addConstructor(ctor);

        cpgf::GMetaAnnotation *annotation = m_accessor.AddItemAnnotation(m_metaClass, new cpgf::GMetaAnnotation(gs::MetaNames::CLASS));
        if (displayName != nullptr) {
            annotation->addItem(gs::MetaNames::DISPLAY_NAME, displayName);
        }
    }
}

void DefineClass::RegisterPin(cpgf::GMetaProperty* property, gs::PinTypes pinType, const char* displayName, gs::TypeInstanceEdit* typeInstance) {
    cpgf::GMetaProperty* prop = m_metaClass->addProperty(property);
    cpgf::GMetaAnnotation *annotation = m_accessor.AddItemAnnotation(prop, new cpgf::GMetaAnnotation(MetaNames::PIN));
    annotation->addItem(MetaNames::PIN_TYPE, pinType);
    if (typeInstance != nullptr) {
        annotation->addItem(MetaNames::TYPE_INSTANCE, typeInstance);
    }
    if (displayName != nullptr) {
        annotation->addItem(MetaNames::DISPLAY_NAME, displayName);
    }
}

DefineEnum::DefineEnum(std::type_index typeIndex) {
    m_metaEnum = new MetaEnum();
    MetaStorage::getInstance().AddEnum(typeIndex, m_metaEnum);
}

DefineEnum::~DefineEnum() noexcept(false) {
    if (m_metaEnum->GetFields().empty()) {
        throw EngineError("gs::DefineEnum: enum without fields");
    }
}

DefineArrayType::DefineArrayType(std::type_index typeIndex) {
    m_metaType = new MetaType();
    MetaStorage::getInstance().AddType(typeIndex, m_metaType);
}

}
