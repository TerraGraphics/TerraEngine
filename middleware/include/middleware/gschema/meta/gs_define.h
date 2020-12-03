#pragma once
// based on cpgf/gmetadefine.h

#include <limits>
#include <vector>
#include <cstdint>
#include <cstddef>
#include <typeindex>
#include <type_traits>
#include <string_view>

#include "core/common/meta.h"
#include "core/common/ctor.h"
#include "cpgf/metaproperty.h"
#include "cpgf/metaannotation.h"
#include "middleware/gschema/meta/gs_meta_type.h"
#include "middleware/gschema/meta/gs_enum_type.h"
#include "middleware/gschema/meta/gs_meta_class.h"
#include "middleware/gschema/meta/gs_meta_consts.h"
#include "middleware/gschema/meta/gs_meta_storage.h"
#include "middleware/gschema/meta/gs_type_instance.h"
#include "middleware/gschema/meta/gs_composite_type.h"
#include "middleware/gschema/meta/gs_arithmetic_type.h"
#include "middleware/gschema/meta/gs_meta_property_data.h"


#define DEFINE_TYPES_IMPL(cls) template <> void GSAutoRegisterTypesFuncT<cls>()
#define DEFINE_CLASSES_IMPL(cls) template <> void GSAutoRegisterClassesFuncT<cls>()


namespace gs {
namespace detail {

template<typename T, typename TBase>
class DefineEnumTypePin : public TBase {
public:
    DefineEnumTypePin() = delete;
    DefineEnumTypePin(MetaClass* metaClass, EnumType<T>* enumType)
        : TBase(metaClass)
        , m_enumType(enumType) {

    }

    DefineEnumTypePin& HideInGUI() {
        m_enumType->HideInGUI();
        return *this;
    }

private:
    EnumType<T>* m_enumType;
};

template<typename T, typename TBase>
class DefineArithmeticTypePin : public TBase {
public:
    DefineArithmeticTypePin() = delete;
    DefineArithmeticTypePin(MetaClass* metaClass, ArithmeticType<T>* arithmeticType)
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

    DefineArithmeticTypePin& HideInGUI() {
        m_arithmeticType->HideInGUI();
        return *this;
    }

    DefineArithmeticTypePin& HideStepButtons() {
        m_arithmeticType->HideStepButtons();
        return *this;
    }

private:
    ArithmeticType<T>* m_arithmeticType;
};

class DefineClass {
protected:
    DefineClass(MetaClass* metaClass);

public:
    DefineClass() = delete;
    DefineClass(std::type_index id, MetaClass* metaClass);

public:
    template <
        typename Getter,
        typename Setter,
        typename T = typename meta::MemberFuncReturnType<Getter>::type,
        std::enable_if_t<std::is_enum_v<T>, int> = 0
        >
    DefineEnumTypePin<T, DefineClass> AddEmbeddedPinEnum(
        std::string_view name, const Getter& getter, const Setter& setter, std::string_view displayName = std::string_view()) {

        auto typeIndex = std::type_index(typeid(T));
        auto* metaEnum = MetaStorage::GetInstance().GetEnum(typeIndex);
        auto* enumType = new EnumType<T>(metaEnum);
        auto* typeInstance = new TypeInstanceEdit(enumType);
        auto* data = new MetaPropertyData<Getter, Setter, cpgf::GMetaPolicyDefault>(getter, setter);
        m_metaClass->AddProperty(data, typeIndex, name, displayName, PinTypes::EMBEDDED, typeInstance);

        return DefineEnumTypePin<T, DefineClass>(m_metaClass, enumType);
    }

    template <
        typename Getter,
        typename Setter,
        typename T = typename meta::MemberFuncReturnType<Getter>::type,
        std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, int> = 0
        >
    DefineArithmeticTypePin<T, DefineClass> AddEmbeddedPinArithmetic(
        std::string_view name, const Getter& getter, const Setter& setter, std::string_view displayName = std::string_view()) {

        auto typeIndex = std::type_index(typeid(T));
        auto* arithmeticType = new ArithmeticType<T>();
        auto* typeInstance = new TypeInstanceEdit(arithmeticType);
        auto* data = new MetaPropertyData<Getter, Setter, cpgf::GMetaPolicyDefault>(getter, setter);
        m_metaClass->AddProperty(data, typeIndex, name, displayName, PinTypes::EMBEDDED, typeInstance);

        return DefineArithmeticTypePin<T, DefineClass>(m_metaClass, arithmeticType);
    }

    template <
        typename Getter,
        typename Setter,
        typename T = typename meta::MemberFuncReturnType<Getter>::type,
        std::enable_if_t<meta::IsArrayLikeV<T>, int> = 0
        >
    DefineClass& AddEmbeddedPinArray(
        std::string_view name, const Getter& getter, const Setter& setter, std::string_view displayName = std::string_view()) {

        auto typeIndex = std::type_index(typeid(T));
        auto* typeInstance = CreateCompositeTypeInstance<T>();
        auto* data = new MetaPropertyData<Getter, Setter, cpgf::GMetaPolicyDefault>(getter, setter);
        m_metaClass->AddProperty(data, typeIndex, name, displayName, PinTypes::EMBEDDED, typeInstance);

        return *this;
    }

    template <
        typename Getter,
        typename Setter,
        typename T = typename meta::MemberFuncReturnType<Getter>::type
        >
    DefineClass& AddInputPin(std::string_view name, const Getter& getter, const Setter& setter, std::string_view displayName = std::string_view()) {
        auto typeIndex = std::type_index(typeid(T));
        auto* data = new MetaPropertyData<Getter, Setter, cpgf::GMetaPolicyDefault>(getter, setter);
        m_metaClass->AddProperty(data, typeIndex, name, displayName, PinTypes::INPUT, nullptr);

        return *this;
    }

    template <
        typename Getter,
        typename T = typename meta::MemberFuncReturnType<Getter>::type
        >
    DefineClass& AddOutputPin(std::string_view name, const Getter& getter, std::string_view displayName = std::string_view()) {
        auto typeIndex = std::type_index(typeid(T));
        auto* data = new MetaPropertyData<Getter, int, cpgf::GMetaPolicyDefault>(getter, 0);
        m_metaClass->AddProperty(data, typeIndex, name, displayName, PinTypes::OUTPUT, nullptr);

        return *this;
    }

private:
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
    MetaClass* m_metaClass = nullptr;
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

template<typename T> void* Ctor() {
    return reinterpret_cast<void*>(new T());
}

template<typename T> void Dtor(void* instance) {
    T* tInstance = reinterpret_cast<T*>(instance);
    delete tInstance;
}

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

template <typename ClassType>
detail::DefineClass DefineBaseClass() {
    return detail::DefineClass(std::type_index(typeid(ClassType)), new MetaClass());
}

template <typename ClassType, typename BaseType0 = void>
detail::DefineClass DefineClass(std::string_view className, std::string_view displayName = std::string_view()) {
    MetaClass* baseClass = nullptr;
    if constexpr (!std::is_same_v<BaseType0, void>) {
        baseClass = MetaStorage::GetInstance().GetBaseClass(std::type_index(typeid(BaseType0)));
    }

    auto* metaClass = new MetaClass(&detail::Ctor<ClassType>, &detail::Dtor<ClassType>, className, displayName, baseClass);
    return detail::DefineClass(std::type_index(typeid(ClassType)), metaClass);
}

} // namespace gs
