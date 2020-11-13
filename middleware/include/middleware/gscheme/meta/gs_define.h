#pragma once
// based on cpgf/gmetadefine.h

#include "cpgf/metaclass.h"
#include "core/common/meta.h"
#include "cpgf/metaproperty.h"
#include "cpgf/metaannotation.h"
#include "middleware/gscheme/meta/gs_meta_type.h"
#include "middleware/gscheme/meta/gs_meta_consts.h"
#include "middleware/gscheme/meta/gs_meta_storage.h"
#include "middleware/gscheme/meta/gs_type_instance.h"
#include "middleware/gscheme/meta/gs_composite_type.h"
#include "middleware/gscheme/meta/gs_primitive_type.h"


namespace cpgf {

template <typename ClassType, typename DerivedType = void>
class GDefineMetaCommon {
public:
	GDefineMetaCommon(meta_internal::GMetaSuperList* superList, const char* className, const char* displayName, bool isBaseClass) {
		m_metaClass = const_cast<GMetaClass *>(getGlobalMetaClass()->doGetClass(className));
		if(m_metaClass == nullptr) {
			ClassType* classType = nullptr;
			const auto policy = GMetaPolicyDefault();
			m_metaClass = new GMetaClass(classType, superList, className, nullptr, policy);
			getGlobalMetaClass()->addClass(m_metaClass);
		}

		if (!isBaseClass) {
			m_metaClass->addConstructor(GMetaConstructor::newConstructor<ClassType, void* ()>(GMetaPolicyDefault()));

			GMetaAnnotation *annotation = m_metaClass->addItemAnnotation(new GMetaAnnotation(gs::MetaNames::CLASS));
			if (displayName != nullptr) {
				annotation->addItem(gs::MetaNames::DISPLAY_NAME, displayName);
			}
		}
	}

public:
	template <
		typename Getter,
		typename Setter,
		typename T = typename meta::MemberFuncReturnType<Getter>::type,
		std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, int> = 0
	>
	GDefineMetaCommon& AddEmbeddedPin(const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {
		auto* primitiveType = new gs::PrimitiveType<T>();
    	auto* typeInstance = new gs::TypeInstanceEdit(primitiveType);
		return AddEmbeddedPinImpl(name, getter, setter, typeInstance, displayName);
	}

	template <
		typename Getter,
		typename Setter,
		typename T = typename meta::MemberFuncReturnType<Getter>::type,
		std::enable_if_t<!std::is_integral_v<T> && !std::is_floating_point_v<T>, int> = 0
	>
	GDefineMetaCommon& AddEmbeddedPin(const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {
		using TCompositeType = gs::CompositeType<T>;
		using TProperty = typename TCompositeType::CompositeTypeItem;
		using TPrimitiveType = gs::PrimitiveType<typename TCompositeType::FieldType>;

		gs::MetaType* metaType = gs::MetaStorage::getInstance().GetType(std::type_index(typeid(T)));

		std::vector<TProperty> properties;
		for (const auto& metaField: metaType->GetFields()) {
			properties.push_back(TProperty{metaField.index, metaField.name, new TPrimitiveType()});
		}

		auto* compositeType = new TCompositeType(properties);
		auto* typeInstance = new gs::TypeInstanceEdit(compositeType);
		return AddEmbeddedPinImpl(name, getter, setter, typeInstance, displayName);
	}

private:
	template <typename Getter, typename Setter>
	GDefineMetaCommon& AddEmbeddedPinImpl(const char* name, const Getter& getter, const Setter& setter, gs::TypeInstanceEdit* typeInstance, const char* displayName = nullptr) {
		GMetaProperty* prop = m_metaClass->addProperty(new GMetaProperty(name, getter, setter, GMetaPolicyDefault()));
		GMetaAnnotation *annotation = prop->addItemAnnotation(new GMetaAnnotation(gs::MetaNames::PIN));
		annotation->addItem(gs::MetaNames::PIN_TYPE, gs::PinTypes::EMBEDDED);
		annotation->addItem(gs::MetaNames::TYPE_INSTANCE, typeInstance);
		if (displayName != nullptr) {
			annotation->addItem(gs::MetaNames::DISPLAY_NAME, displayName);
		}

		return *this;
	}

public:
	template <typename Getter, typename Setter>
	GDefineMetaCommon& AddInputPin(const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {
		GMetaProperty* prop = m_metaClass->addProperty(new GMetaProperty(name, getter, setter, GMetaPolicyDefault()));
		GMetaAnnotation *annotation = prop->addItemAnnotation(new GMetaAnnotation(gs::MetaNames::PIN));
		annotation->addItem(gs::MetaNames::PIN_TYPE, gs::PinTypes::INPUT);
		if (displayName != nullptr) {
			annotation->addItem(gs::MetaNames::DISPLAY_NAME, displayName);
		}

		return *this;
	}

	template <typename Getter>
	GDefineMetaCommon& AddOutputPin(const char* name, const Getter& getter, const char* displayName = nullptr) {
		GMetaProperty* prop = m_metaClass->addProperty(new GMetaProperty(name, getter, 0, GMetaPolicyDefault()));
		GMetaAnnotation *annotation = prop->addItemAnnotation(new GMetaAnnotation(gs::MetaNames::PIN));
		annotation->addItem(gs::MetaNames::PIN_TYPE, gs::PinTypes::OUTPUT);
		if (displayName != nullptr) {
			annotation->addItem(gs::MetaNames::DISPLAY_NAME, displayName);
		}

		return *this;
	}

protected:
	GMetaClass* m_metaClass = nullptr;
};

} // namespace cpgf

namespace gs {

namespace detail {

class DefineArrayType : Fixed {
public:
	DefineArrayType() = delete;
	DefineArrayType(MetaType* metaType)
		: m_metaType(metaType) {

	}

	template<typename T>
	DefineArrayType& FieldByIndex(ptrdiff_t index, std::string_view name) {
		m_metaType->AddFieldByIndex(index, name, std::type_index(typeid(T)));

		return *this;
	}

private:
	MetaType* m_metaType = nullptr;
};

template <typename ClassType, typename BaseType0>
cpgf::meta_internal::GMetaSuperList* createSuperList() {
	cpgf::meta_internal::GMetaSuperList* superList = new cpgf::meta_internal::GMetaSuperList();
	superList->add<ClassType, BaseType0>();
	// superList->add<ClassType, BaseType1>();
	// ...

	return superList;
}

}

template<typename T, std::enable_if_t<meta::IsArrayLikeV<T>, int> = 0>
detail::DefineArrayType DefineType() {
	auto* metaType = new MetaType();
	MetaStorage::getInstance().AddType(std::type_index(typeid(T)), metaType);

	return detail::DefineArrayType(metaType);
}

template <typename ClassType, typename BaseType0 = void>
cpgf::GDefineMetaCommon<ClassType> DefineBaseClass(const char* className) {
	auto* superList = detail::createSuperList<ClassType, BaseType0>();
	return cpgf::GDefineMetaCommon<ClassType>(superList, className, nullptr, true);
}

template <typename ClassType, typename BaseType0 = void>
cpgf::GDefineMetaCommon<ClassType> DefineClass(const char* className, const char* displayName = nullptr) {
	auto* superList = detail::createSuperList<ClassType, BaseType0>();
	return cpgf::GDefineMetaCommon<ClassType>(superList, className, displayName, false);
}

} // namespace gs
