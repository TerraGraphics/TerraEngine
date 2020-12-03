#include "middleware/gschema/meta/gs_define.h"
#include "middleware/gschema/meta/gs_define_decl.h"

#include "core/common/exception.h"


namespace gs::detail {

int RegisterCallbacksImpl(void(*types)(), void(*classes)()) {
    MetaStorage::GetInstance().AddDefineTypesCallback(types);
	MetaStorage::GetInstance().AddDefineClassesCallback(classes);

    return 0;
}

DefineClass::DefineClass(MetaClass* metaClass)
    : m_metaClass(metaClass) {

}

DefineClass::DefineClass(std::type_index id, MetaClass* metaClass)
    : m_metaClass(metaClass) {

    if (metaClass->IsBaseClass()) {
        MetaStorage::GetInstance().AddBaseClass(id, metaClass);
    } else {
        MetaStorage::GetInstance().AddClass(id, metaClass);
    }
}

DefineEnum::DefineEnum(std::type_index typeIndex) {
    m_metaEnum = new MetaEnum();
    MetaStorage::GetInstance().AddEnum(typeIndex, m_metaEnum);
}

DefineEnum::~DefineEnum() noexcept(false) {
    if (m_metaEnum->GetFields().empty()) {
        throw EngineError("gs::DefineEnum: enum without fields");
    }
}

DefineArrayType::DefineArrayType(std::type_index typeIndex) {
    m_metaType = new MetaType();
    MetaStorage::GetInstance().AddType(typeIndex, m_metaType);
}

}
