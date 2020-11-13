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

template <typename ClassType, typename DerivedType> class GDefineMetaCommon;

template <>
class GDefineMetaCommon<void, void> {
public:
	GMetaAnnotation* AddItemAnnotation(GMetaItem* item, GMetaAnnotation* annotation) {
		return item->addItemAnnotation(annotation);
	}
};

template <typename ClassType, typename BaseType0>
GMetaClass* createMetaClass(const char* className) {
	auto* superList = new meta_internal::GMetaSuperList();
	superList->add<ClassType, BaseType0>();
	// superList->add<ClassType, BaseType1>();
	// ...

	ClassType* classType = nullptr;
	const auto policy = GMetaPolicyDefault();
	return new GMetaClass(classType, superList, className, nullptr, policy);
}

}

namespace gs {
namespace detail {

class DefineClassBase {
protected:
	DefineClassBase() = delete;
	DefineClassBase(cpgf::GMetaClass* metaClass, cpgf::GMetaConstructor* ctor, const char* displayName, bool isBaseClass);

protected:
	void RegisterPin(cpgf::GMetaProperty* property, PinTypes pinType, const char* displayName, TypeInstanceEdit* typeInstance);

protected:
	cpgf::GMetaClass* m_metaClass = nullptr;
	cpgf::GDefineMetaCommon<void, void> m_accessor;
};

class DefineClass final : public DefineClassBase {
public:
	DefineClass(cpgf::GMetaClass* metaClass, cpgf::GMetaConstructor* ctor, const char* displayName, bool isBaseClass)
		: gs::detail::DefineClassBase(metaClass, ctor, displayName, isBaseClass) {
	}

public:
	template <
		typename Getter,
		typename Setter,
		typename T = typename meta::MemberFuncReturnType<Getter>::type,
		std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, int> = 0
	>
	DefineClass& AddEmbeddedPin(const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {
		auto* primitiveType = new PrimitiveType<T>();
    	auto* typeInstance = new TypeInstanceEdit(primitiveType);
		auto* metaPropoperty = new cpgf::GMetaProperty(name, getter, setter, cpgf::GMetaPolicyDefault());
		RegisterPin(metaPropoperty, PinTypes::EMBEDDED, displayName, typeInstance);

		return *this;
	}

	template <
		typename Getter,
		typename Setter,
		typename T = typename meta::MemberFuncReturnType<Getter>::type,
		std::enable_if_t<!std::is_integral_v<T> && !std::is_floating_point_v<T>, int> = 0
	>
	DefineClass& AddEmbeddedPin(const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {
		using TCompositeType = CompositeType<T>;
		using TProperty = typename TCompositeType::CompositeTypeItem;
		using TPrimitiveType = PrimitiveType<typename TCompositeType::FieldType>;

		MetaType* metaType = MetaStorage::getInstance().GetType(std::type_index(typeid(T)));

		std::vector<TProperty> properties;
		for (const auto& metaField: metaType->GetFields()) {
			properties.push_back(TProperty{metaField.index, metaField.name, new TPrimitiveType()});
		}

		auto* compositeType = new TCompositeType(properties);
		auto* typeInstance = new TypeInstanceEdit(compositeType);
		auto* metaPropoperty = new cpgf::GMetaProperty(name, getter, setter, cpgf::GMetaPolicyDefault());
		RegisterPin(metaPropoperty, PinTypes::EMBEDDED, displayName, typeInstance);

		return *this;
	}

	template <typename Getter, typename Setter>
	DefineClass& AddInputPin(const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {
		auto* metaPropoperty = new cpgf::GMetaProperty(name, getter, setter, cpgf::GMetaPolicyDefault());
		RegisterPin(metaPropoperty, PinTypes::INPUT, displayName, nullptr);

		return *this;
	}

	template <typename Getter>
	DefineClass& AddOutputPin(const char* name, const Getter& getter, const char* displayName = nullptr) {
		auto* metaPropoperty = new cpgf::GMetaProperty(name, getter, 0, cpgf::GMetaPolicyDefault());
		RegisterPin(metaPropoperty, PinTypes::OUTPUT, displayName, nullptr);

		return *this;
	}
};

class DefineArrayType : Fixed {
public:
	DefineArrayType() = delete;
	DefineArrayType(std::type_index typeIndex) {
		m_metaType = new MetaType();
		MetaStorage::getInstance().AddType(typeIndex, m_metaType);
	}

	template<typename T>
	DefineArrayType& FieldByIndex(ptrdiff_t index, std::string_view name) {
		m_metaType->AddFieldByIndex(index, name, std::type_index(typeid(T)));

		return *this;
	}

private:
	MetaType* m_metaType = nullptr;
};

}

template<typename T, std::enable_if_t<meta::IsArrayLikeV<T>, int> = 0>
detail::DefineArrayType DefineType() {
	return detail::DefineArrayType(std::type_index(typeid(T)));
}

template <typename ClassType, typename BaseType0 = void>
detail::DefineClass DefineBaseClass(const char* className) {
	return detail::DefineClass(cpgf::createMetaClass<ClassType, BaseType0>(className), nullptr, nullptr, true);
}

template <typename ClassType, typename BaseType0 = void>
detail::DefineClass DefineClass(const char* className, const char* displayName = nullptr) {
	auto* ctor = cpgf::GMetaConstructor::newConstructor<ClassType, void* ()>(cpgf::GMetaPolicyDefault());
	return detail::DefineClass(cpgf::createMetaClass<ClassType, BaseType0>(className), ctor, displayName, false);
}

} // namespace gs
