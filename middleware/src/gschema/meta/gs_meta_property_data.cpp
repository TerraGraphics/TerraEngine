#include "middleware/gschema/meta/gs_meta_property_data.h"

#include "core/common/exception.h"


namespace gs {

namespace detail {

void ReadForbidden() {
	throw EngineError("Can't read object. Read is forbidden");
}

void WriteForbidden() {
	throw EngineError("Can't write object. Write is forbidden");
}

}

void MetaPropertyDataBase::DeleteSelf() {
	m_vTable->DeleteSelf(this);
}

cpgf::GVariant MetaPropertyDataBase::Get(const void * instance) const {
	return m_vTable->Get(this, instance);
}

void MetaPropertyDataBase::Set(void * instance, const cpgf::GVariant & v) const {
	m_vTable->Set(this, instance, v);
}

}
