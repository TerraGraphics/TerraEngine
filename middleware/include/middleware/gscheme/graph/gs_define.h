#pragma once

// based on cpgf/gmetadefine.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#if defined(__GNUC__)
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wredundant-decls"
#endif

#include <cpgf/gmetaclass.h>
#include <cpgf/gmetaproperty.h>
#include <cpgf/gmetaannotation.h>

#pragma GCC diagnostic pop

#include <charconv>

#include "fmt/fmt.h"
#include "core/common/meta.h"
#include "middleware/gscheme/meta/gs_meta_type.h"
#include "middleware/gscheme/graph/gs_metadata.h"
#include "middleware/gscheme/meta/gs_meta_storage.h"


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

	template <typename Getter, typename Setter>
	GDefineMetaCommon& AddEmbeddedPin(const char* name, const Getter& getter, const Setter& setter, const char* displayName = nullptr) {
		GMetaProperty* prop = m_metaClass->addProperty(new GMetaProperty(name, getter, setter, GMetaPolicyDefault()));
		GMetaAnnotation *annotation = prop->addItemAnnotation(new GMetaAnnotation(gs::MetaNames::PIN));
		annotation->addItem(gs::MetaNames::PIN_TYPE, gs::PinTypes::EMBEDDED);
		if (displayName != nullptr) {
			annotation->addItem(gs::MetaNames::DISPLAY_NAME, displayName);
		}

		return *this;
	}

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
	DefineArrayType& FieldByIndex(size_t index, std::string_view name) {
		m_metaType->AddFieldByIndex(index, name, MetaStorage::getInstance().GetType(std::type_index(typeid(T))));

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

template<typename T, std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, int> = 0>
void DefineType() {
	auto* metaType = new MetaType(
		[](const cpgf::GVariant& v) -> std::string {
			fmt::memory_buffer buffer;
			fmt::format_to(buffer, "{}", cpgf::fromVariant<T>(v));
			return buffer.data();
		},
		[](const std::string& in, cpgf::GVariant& out) -> bool {
			if constexpr (std::is_same_v<T, float>) {
				try {
					std::size_t pos;
					out = std::stof(in, &pos);
					return true;
				} catch(const std::out_of_range&) {
					return false;
				} catch(const std::invalid_argument&) {
					return false;
				}
			} else if constexpr (std::is_same_v<T, double>) {
				try {
					std::size_t pos;
					out = std::stod(in, &pos);
					return true;
				} catch(const std::out_of_range&) {
					return false;
				} catch(const std::invalid_argument&) {
					return false;
				}
			} else {
				T tmp;
				if(auto [p, ec] = std::from_chars(in.data(), in.data() + in.size(), tmp); ec == std::errc()) {
					out = tmp;
					return true;
				}

				return false;
			}
		}
	);

	MetaStorage::getInstance().AddType(std::type_index(typeid(T)), metaType);
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
