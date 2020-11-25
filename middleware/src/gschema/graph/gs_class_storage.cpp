#include "middleware/gschema/graph/gs_class_storage.h"

#include <vector>
#include <utility>
#include <cstddef>
#include <typeindex>
#include <unordered_map>

#include "eigen/core.h"
#include "cpgf/metaclass.h"
#include "core/common/hash.h"
#include "core/common/exception.h"
#include "middleware/generator/gs_register.h" // IWYU pragma: keep
#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/graph/gs_class.h"
#include "middleware/gschema/meta/gs_meta_type.h"
#include "middleware/gschema/embedded/embedded.h" // IWYU pragma: keep
#include "middleware/gschema/graph/gs_types_fmt.h" // IWYU pragma: keep
#include "middleware/gschema/meta/gs_meta_consts.h"
#include "middleware/gschema/meta/gs_meta_storage.h"
#include "middleware/gschema/meta/gs_type_instance.h"
#include "middleware/gschema/meta/gs_composite_type.h"
#include "middleware/gschema/meta/gs_arithmetic_type.h"
#include "middleware/gschema/graph/gs_types_convert_storage.h"


namespace gs {

struct ClassStorage::Impl {
    Impl();
    ~Impl();

    void GenerateTypeInstances();

    uint16_t m_countClasses = 0;
    Class* m_classes;
    TypesConvertStorage m_typesConvertStorage;
    std::unordered_map<TypeId, TypeInstanceEdit*> m_typeInstances;
    std::unordered_map<std::string_view, uint16_t> m_classesIndex;
};

ClassStorage::Impl::Impl() {
    GenerateTypeInstances();

    const cpgf::GMetaClass* gMetaClass = cpgf::getGlobalMetaClass();

    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if (metaClass == nullptr) {
            continue;
        }
        if (metaClass->getAnnotation(MetaNames::CLASS) != nullptr) {
            ++m_countClasses;
        }
    }

    uint16_t index = 0;
    m_classes = new Class[m_countClasses];
    for(size_t i=0; i!=gMetaClass->getClassCount(); ++i) {
        const cpgf::GMetaClass* metaClass = gMetaClass->getClassAt(i);
        if ((metaClass != nullptr) && (metaClass->getAnnotation(MetaNames::CLASS) != nullptr)) {
            m_classes[index].Create(metaClass, &m_typesConvertStorage, &m_typeInstances);
            m_classesIndex[m_classes[index].GetName()] = index;
            ++index;
        }
    }
}
ClassStorage::Impl::~Impl() {
    m_classesIndex.clear();
    if (m_classes != nullptr) {
        delete[] m_classes;
    }
    for (auto [_, typeInstance]: m_typeInstances) {
        delete typeInstance;
    }
}

template<typename T>
TypeInstanceEdit* CreateCompositeTypeInstance() {
    constexpr char prettyNames[] = "RGBA";
    using TCompositeType = CompositeTypeT<T>;
    using TProperty = typename TCompositeType::CompositeTypeItem;
    using TArithmeticType = ArithmeticType<typename TCompositeType::FieldType>;

    MetaType* metaType = MetaStorage::getInstance().GetType(std::type_index(typeid(T)));

    std::vector<TProperty> properties;
    for (const auto& metaField: metaType->GetFields()) {
        auto* arithmeticType = new TArithmeticType();
        if (metaField.index >= 4) {
            throw EngineError("gs::ClassStorage: unknown metaField.index = {}, for generate TypeInstance", metaField.index);
        }
        arithmeticType->SetPrettyName(std::string_view(&prettyNames[metaField.index], 1));
        properties.push_back(TProperty{metaField.index, arithmeticType});
    }

    auto* compositeType = new TCompositeType(properties);
    return new TypeInstanceEdit(compositeType);
}

void ClassStorage::Impl::GenerateTypeInstances() {
    for(TypeId typeId = GetBeginBaseType(); typeId != GetEndBaseType(); typeId = GetNextBaseType(typeId)) {
        if (!IsEnableUI(typeId)) {
            continue;
        }
        if (typeId == TypeId::Float) {
            auto* arithmeticType = new ArithmeticType<float>();
            arithmeticType->SetPrettyName("R");
            m_typeInstances[typeId] = new TypeInstanceEdit(arithmeticType);
        } else if (typeId == TypeId::Vector2f) {
            m_typeInstances[typeId] = CreateCompositeTypeInstance<Eigen::Vector2f>();
        } else if (typeId == TypeId::Vector3f) {
            m_typeInstances[typeId] = CreateCompositeTypeInstance<Eigen::Vector3f>();
        } else if (typeId == TypeId::Vector4f) {
            m_typeInstances[typeId] = CreateCompositeTypeInstance<Eigen::Vector4f>();
        } else {
            throw EngineError("gs::ClassStorage: unknown typeId = {}, for generate TypeInstance", typeId);
        }
    }
}

ClassStorage::ClassStorage() {

}

ClassStorage::~ClassStorage() {

}

Class* ClassStorage::GetClass(uint16_t classIndex) {
    if (classIndex >= impl->m_countClasses) {
        throw EngineError(
            "gs::ClassStorage::GetClass: wrong classIndex = {}, max value = {}", classIndex, impl->m_countClasses - 1);
    }

    return &impl->m_classes[classIndex];
}

Class* ClassStorage::GetClass(std::string_view name) {
    return GetClass(GetClassIndex(name));
}

uint16_t ClassStorage::GetClassIndex(std::string_view name) {
    if (auto it = impl->m_classesIndex.find(name); it != impl->m_classesIndex.cend()) {
        return it->second;
    }

    throw EngineError("gs::ClassStorage::GetClassIndex: wrong name = {}, not found node with this name", name);
}

const Class* ClassStorage::ClassesBegin() const noexcept {
    return impl->m_classes;
}

const Class* ClassStorage::ClassesEnd() const noexcept {
    return impl->m_classes + impl->m_countClasses;
}

}
