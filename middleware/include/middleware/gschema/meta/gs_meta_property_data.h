#pragma once

#include "cpgf/variant.h"
#include "cpgf/accessor.h"


namespace gs {

namespace detail {
	void ReadForbidden();
	void WriteForbidden();
}

struct MetaPropertyDataVTable {
	void (*Release)(void* self);
	cpgf::GVariant (*Get)(const void* self, const void* instance);
	void (*Set)(const void* self, void* instance, const cpgf::GVariant& v);
};

class MetaPropertyDataBase {
public:
	void Release();
	cpgf::GVariant Get(const void* instance) const;
	void Set(void* instance, const cpgf::GVariant& v) const;

protected:
	MetaPropertyDataVTable* m_vTable = nullptr;
};

template <typename Getter, typename Setter>
class MetaPropertyData : public MetaPropertyDataBase {
private:
	using TGetter = cpgf::getter_internal::GInstanceGetterImplement<Getter, cpgf::GMetaPolicyDefault>;
	using TSetter = cpgf::setter_internal::GInstanceSetterImplement<Setter, cpgf::GMetaPolicyDefault>;

private:
	static void VirtualRelease(void* self) {
		delete static_cast<MetaPropertyData *>(self);
	}

	static cpgf::GVariant VirtualGet(const void* self, const void* instance) {
		if constexpr (TGetter::HasGetter && TGetter::Readable) {
			return cpgf::createVariant<typename TGetter::ValueType>(TGetter::get(static_cast<const MetaPropertyData *>(self)->m_getter, instance), true);
		} else {
			detail::ReadForbidden();
			return cpgf::GVariant();
		}
	}

	static void VirtualSet(const void* self, void* instance, const cpgf::GVariant& value) {
		if constexpr (TSetter::HasSetter && TSetter::Writable) {
			TSetter::set(static_cast<const MetaPropertyData *>(self)->m_setter, instance, cpgf::fromVariant<typename TSetter::PassType>(value));
		} else {
			detail::WriteForbidden();
		}
	}

public:
	MetaPropertyData(const Getter& getter, const Setter& setter)
		: m_getter(getter)
		, m_setter(setter) {

		static MetaPropertyDataVTable vTable = {
			&VirtualRelease,
			&VirtualGet,
			&VirtualSet
		};
		m_vTable = &vTable;
	}

private:
	mutable typename TGetter::DataType m_getter;
	mutable typename TSetter::DataType m_setter;
};

}
