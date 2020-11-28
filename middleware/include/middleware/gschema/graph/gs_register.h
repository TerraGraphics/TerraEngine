#pragma once

#include "core/common/macros.h"
#include "middleware/gschema/meta/gs_meta_storage.h"


namespace gs::detail {

template <typename T> inline int RegisterCallbacks(void (*types)(), void (*classes)()) {
	static const int s = [&types, &classes]() {
		MetaStorage::getInstance().AddDefineTypesCallback(types);
		MetaStorage::getInstance().AddDefineClassesCallback(classes);
		return 0;
	}();
	return s;
}

}

#define DEFINE_DECL(cls)                                            \
	template <typename T> extern void GSAutoRegisterTypesFuncT();   \
	template <> void GSAutoRegisterTypesFuncT<cls>();               \
	template <typename T> extern void GSAutoRegisterClassesFuncT(); \
	template <> void GSAutoRegisterClassesFuncT<cls>();             \
	static const int UNIQUE_VAR(auto_register__) =                  \
		gs::detail::RegisterCallbacks<cls>(&GSAutoRegisterTypesFuncT<cls>, &GSAutoRegisterClassesFuncT<cls>)

#define DEFINE_TYPES_IMPL(cls) template <> void GSAutoRegisterTypesFuncT<cls>()
#define DEFINE_CLASSES_IMPL(cls) template <> void GSAutoRegisterClassesFuncT<cls>()
