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

template<typename T, typename TBase>
class DefinePrimitiveTypePin : public TBase {
public:
	DefinePrimitiveTypePin() = delete;
	DefinePrimitiveTypePin(cpgf::GMetaClass* metaClass, PrimitiveType<T>* primitiveType)
		: TBase(metaClass)
		, m_primitiveType(primitiveType) {

	}

	DefinePrimitiveTypePin& Max(T value) {
		m_primitiveType->Max(value);
		return *this;
    }

    DefinePrimitiveTypePin& Min(T value) {
		m_primitiveType->Min(value);
		return *this;
    }

    DefinePrimitiveTypePin& LimitFunc(const typename PrimitiveType<T>::TLimitFunc func) {
		m_primitiveType->TLimitFunc(func);
		return *this;
    }

    DefinePrimitiveTypePin& MaxPrecision(uint8_t value) {
		m_primitiveType->MaxPrecision(value);
		return *this;
    }

    DefinePrimitiveTypePin& DisableUI() {
		m_primitiveType->DisableUI();
		return *this;
    }

private:
	PrimitiveType<T>* m_primitiveType;
};

class DefineClass {
protected:
	DefineClass(cpgf::GMetaClass* metaClass);

public:
	DefineClass() = delete;
	DefineClass(cpgf::GMetaClass* metaClass, cpgf::GMetaConstructor* ctor, const char* displayName, bool isBaseClass);

public:
	template <
		typename Getter,
		typename Setter,
		typename T = typename meta::MemberFuncReturnType<Getter>::type,
		std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, int> = 0
		>
	DefinePrimitiveTypePin<T, DefineClass> AddEmbeddedPinPrimitive(
		const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {

		auto* primitiveType = new PrimitiveType<T>();
		auto* typeInstance = new TypeInstanceEdit(primitiveType);
		auto* metaPropoperty = new cpgf::GMetaProperty(name, getter, setter, cpgf::GMetaPolicyDefault());
		RegisterPin(metaPropoperty, PinTypes::EMBEDDED, displayName, typeInstance);

		return DefinePrimitiveTypePin<T, DefineClass>(m_metaClass, primitiveType);
	}

	template <
		typename Getter,
		typename Setter,
		typename T = typename meta::MemberFuncReturnType<Getter>::type,
		std::enable_if_t<meta::IsArrayLikeV<T>, int> = 0
		>
	DefineClass& AddEmbeddedPinArray(
		const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {

		auto* typeInstance = CreateCompositeTypeInstance<T>();
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

private:
	void RegisterPin(cpgf::GMetaProperty* property, PinTypes pinType, const char* displayName, TypeInstanceEdit* typeInstance);

	template<typename T>
	TypeInstanceEdit* CreateCompositeTypeInstance() {
		using TCompositeType = CompositeType<T>;
		using TProperty = typename TCompositeType::CompositeTypeItem;
		using TPrimitiveType = PrimitiveType<typename TCompositeType::FieldType>;

		MetaType* metaType = MetaStorage::getInstance().GetType(std::type_index(typeid(T)));

		std::vector<TProperty> properties;
		for (const auto& metaField: metaType->GetFields()) {
			properties.push_back(TProperty{metaField.index, metaField.name, new TPrimitiveType()});
		}

		auto* compositeType = new TCompositeType(properties);
		return new TypeInstanceEdit(compositeType);
	}

private:
	cpgf::GMetaClass* m_metaClass = nullptr;
	cpgf::GDefineMetaCommon<void, void> m_accessor;
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
