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
	using TGetter = cpgf::meta_internal::GMetaGetter<Getter, Policy>;
	using TSetter = cpgf::meta_internal::GMetaSetter<Setter, Policy>;

private:
	static cpgf::GVariant VirtualGet(const void* self, const void* instance) {
		if (!TGetter::Readable) {
			detail::ReadForbidden();
		}
		return static_cast<const MetaPropertyData *>(self)->m_getter.get(instance);
	}

	static void VirtualSet(const void* self, void* instance, const cpgf::GVariant& value) {
		if (!TSetter::Writable) {
			detail::WriteForbidden();
		}
		static_cast<const MetaPropertyData *>(self)->m_setter.set(instance, value);
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
