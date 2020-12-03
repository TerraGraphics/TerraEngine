#pragma once

#include <string>
#include <typeindex>
#include <string_view>

#include "core/common/ctor.h"
#include "cpgf/metaproperty.h"
#include "middleware/gschema/meta/gs_meta_consts.h"


namespace gs {

namespace detail {
	void ReadForbidden();
	void WriteForbidden();
}

template <typename Getter, typename Policy>
class MetaGetter : public cpgf::GInstanceGetter<Getter, Policy> {
private:
	using TSuper = cpgf::GInstanceGetter<Getter, Policy>;

public:
	MetaGetter(const Getter& getter)
		: TSuper(getter) {
	}

	cpgf::GVariant Get(const void* instance) const {
		return this->DoGet<void>(instance);
	}

private:
	template <typename T>
	cpgf::GVariant DoGet(typename cpgf::GEnableIf<TSuper::Readable, T>::Result const* instance) const {
		return cpgf::createVariant<typename TSuper::ValueType>(TSuper::get(instance), true);
	}

	template <typename T>
	cpgf::GVariant DoGet(typename cpgf::GDisableIf<TSuper::Readable, T>::Result const* /*instance*/) const {
		detail::ReadForbidden();

		return cpgf::GVariant();
	}
};

template <typename Setter, typename Policy>
class MetaSetter : public cpgf::GInstanceSetter<Setter, Policy>
{
private:
	using TSuper = cpgf::GInstanceSetter<Setter, Policy>;

public:
	MetaSetter(const Setter& setter)
		: TSuper(setter) {
	}

	void Set(void* instance, const cpgf::GVariant& value) const {
		TSuper::set(instance, cpgf::fromVariant<typename TSuper::PassType>(value));
	}
};

struct MetaPropertyDataVTable {
	void (*DeleteSelf)(void* self);
	cpgf::GVariant (*Get)(const void* self, const void* instance);
	void (*Set)(const void* self, void* instance, const cpgf::GVariant& v);
};

class MetaPropertyDataBase {
public:
	void DeleteSelf();
	cpgf::GVariant Get(const void* instance) const;
	void Set(void* instance, const cpgf::GVariant& v) const;

protected:
	MetaPropertyDataVTable* m_vTable = nullptr;
};

template <typename Getter, typename Setter, typename Policy>
class MetaPropertyData : public MetaPropertyDataBase {
private:
	using TGetter = MetaGetter<Getter, Policy>;
	using TSetter = MetaSetter<Setter, Policy>;

private:
	static cpgf::GVariant VirtualGet(const void* self, const void* instance) {
		if (!TGetter::Readable) {
			detail::ReadForbidden();
		}
		return static_cast<const MetaPropertyData *>(self)->m_getter.Get(instance);
	}

	static void VirtualSet(const void* self, void* instance, const cpgf::GVariant& value) {
		if (!TSetter::Writable) {
			detail::WriteForbidden();
		}
		static_cast<const MetaPropertyData *>(self)->m_setter.Set(instance, value);
	}

public:
	MetaPropertyData(const Getter& getter, const Setter& setter)
		: m_getter(getter)
		, m_setter(setter) {

		static MetaPropertyDataVTable vTable = {
			&cpgf::meta_internal::virtualBaseMetaDeleter<MetaPropertyData>,
			&VirtualGet,
			&VirtualSet
		};
		m_vTable = &vTable;
	}

private:
	TGetter m_getter;
	TSetter m_setter;
};

}
