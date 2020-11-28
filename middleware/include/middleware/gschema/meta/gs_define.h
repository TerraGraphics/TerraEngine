#pragma once
// based on cpgf/gmetadefine.h

#include <limits>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <typeindex>
#include <type_traits>
#include <string_view>

#include "cpgf/metaclass.h"
#include "core/common/meta.h"
#include "core/common/ctor.h"
#include "cpgf/metaproperty.h"
#include "cpgf/metaannotation.h"
#include "middleware/gschema/meta/gs_meta_type.h"
#include "middleware/gschema/meta/gs_enum_type.h"
#include "middleware/gschema/meta/gs_meta_consts.h"
#include "middleware/gschema/meta/gs_meta_storage.h"
#include "middleware/gschema/meta/gs_type_instance.h"
#include "middleware/gschema/meta/gs_composite_type.h"
#include "middleware/gschema/meta/gs_arithmetic_type.h"


#define DEFINE_TYPES_IMPL(cls) template <> void GSAutoRegisterTypesFuncT<cls>()
#define DEFINE_CLASSES_IMPL(cls) template <> void GSAutoRegisterClassesFuncT<cls>()

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
class DefineEnumTypePin : public TBase {
public:
    DefineEnumTypePin() = delete;
    DefineEnumTypePin(cpgf::GMetaClass* metaClass, EnumType<T>* enumType)
        : TBase(metaClass)
        , m_enumType(enumType) {

    }

    DefineEnumTypePin& DisableUI() {
        m_enumType->DisableUI();
        return *this;
    }

private:
    EnumType<T>* m_enumType;
};

template<typename T, typename TBase>
class DefineArithmeticTypePin : public TBase {
public:
    DefineArithmeticTypePin() = delete;
    DefineArithmeticTypePin(cpgf::GMetaClass* metaClass, ArithmeticType<T>* arithmeticType)
        : TBase(metaClass)
        , m_arithmeticType(arithmeticType) {

    }

    DefineArithmeticTypePin& Step(T value) {
        m_arithmeticType->Step(value);
        return *this;
    }

    DefineArithmeticTypePin& Max(T value) {
        m_arithmeticType->Max(value);
        return *this;
    }

    DefineArithmeticTypePin& Min(T value) {
        m_arithmeticType->Min(value);
        return *this;
    }

    DefineArithmeticTypePin& LimitFunc(const typename ArithmeticType<T>::TLimitFunc limitFunc, const typename ArithmeticType<T>::TStepFunc stepFunc) {
        m_arithmeticType->Funcs(limitFunc, stepFunc);
        return *this;
    }

    DefineArithmeticTypePin& MaxPrecision(uint8_t value) {
        m_arithmeticType->MaxPrecision(value);
        return *this;
    }

    DefineArithmeticTypePin& DisableUI() {
        m_arithmeticType->DisableUI();
        return *this;
    }

    DefineArithmeticTypePin& DisableStepButtons() {
        m_arithmeticType->DisableStepButtons();
        return *this;
    }

private:
    ArithmeticType<T>* m_arithmeticType;
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
        std::enable_if_t<std::is_enum_v<T>, int> = 0
        >
    DefineEnumTypePin<T, DefineClass> AddEmbeddedPinEnum(
        const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {

        auto* metaEnum = MetaStorage::GetInstance().GetEnum(std::type_index(typeid(T)));
        auto* enumType = new EnumType<T>(metaEnum);
        auto* typeInstance = new TypeInstanceEdit(enumType);
        auto* metaPropoperty = new cpgf::GMetaProperty(name, getter, setter, cpgf::GMetaPolicyDefault());
        RegisterPin(metaPropoperty, PinTypes::EMBEDDED, displayName, typeInstance);

        return DefineEnumTypePin<T, DefineClass>(m_metaClass, enumType);
    }

    template <
        typename Getter,
        typename Setter,
        typename T = typename meta::MemberFuncReturnType<Getter>::type,
        std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, int> = 0
        >
    DefineArithmeticTypePin<T, DefineClass> AddEmbeddedPinArithmetic(
        const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {

        auto* arithmeticType = new ArithmeticType<T>();
        auto* typeInstance = new TypeInstanceEdit(arithmeticType);
        auto* metaPropoperty = new cpgf::GMetaProperty(name, getter, setter, cpgf::GMetaPolicyDefault());
        RegisterPin(metaPropoperty, PinTypes::EMBEDDED, displayName, typeInstance);

        return DefineArithmeticTypePin<T, DefineClass>(m_metaClass, arithmeticType);
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
        using TCompositeType = CompositeTypeT<T>;
        using TProperty = typename TCompositeType::CompositeTypeItem;
        using TArithmeticType = ArithmeticType<typename TCompositeType::FieldType>;

        MetaType* metaType = MetaStorage::GetInstance().GetType(std::type_index(typeid(T)));

        std::vector<TProperty> properties;
        for (const auto& metaField: metaType->GetFields()) {
            auto* arithmeticType = new TArithmeticType();
            arithmeticType->SetDisplayName(metaField.name);
            properties.push_back(TProperty{metaField.index, arithmeticType});
        }

        auto* compositeType = new TCompositeType(properties);
        return new TypeInstanceEdit(compositeType);
    }

private:
    cpgf::GMetaClass* m_metaClass = nullptr;
    cpgf::GDefineMetaCommon<void, void> m_accessor;
};

class DefineEnum : Fixed {
public:
    DefineEnum() = delete;
    DefineEnum(std::type_index typeIndex);
    ~DefineEnum() noexcept(false);

    template<typename T>
    DefineEnum& AddField(T value, std::string_view name, std::string_view displayName = std::string_view()) {
        m_metaEnum->AddField(static_cast<MetaEnum::ValueType>(value), name, displayName);

        return *this;
    }

private:
    MetaEnum* m_metaEnum = nullptr;
};

class DefineArrayType : Fixed {
public:
    DefineArrayType() = delete;
    DefineArrayType(std::type_index typeIndex);

    template<typename T>
    DefineArrayType& AddFieldByIndex(ptrdiff_t index, std::string_view name) {
        m_metaType->AddFieldByIndex(index, name, std::type_index(typeid(T)));

        return *this;
    }

private:
    MetaType* m_metaType = nullptr;
};

}

template<typename T, std::enable_if_t<std::is_enum_v<T> &&
    std::numeric_limits<std::underlying_type_t<T>>::max() <= std::numeric_limits<MetaEnum::ValueType>::max() &&
    std::numeric_limits<std::underlying_type_t<T>>::min() >= std::numeric_limits<MetaEnum::ValueType>::min(), int> = 0>
detail::DefineEnum DefineEnum() {
    return detail::DefineEnum(std::type_index(typeid(T)));
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
