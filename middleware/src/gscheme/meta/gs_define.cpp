#include "middleware/gscheme/meta/gs_define.h"


namespace gs::detail {

DefineClassBase::DefineClassBase(cpgf::GMetaClass* metaClass, cpgf::GMetaConstructor* ctor, const char* displayName, bool isBaseClass) {
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

void DefineClassBase::RegisterPin(cpgf::GMetaProperty* property, gs::PinTypes pinType, const char* displayName, gs::TypeInstanceEdit* typeInstance) {
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

}
