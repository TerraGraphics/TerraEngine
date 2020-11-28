#pragma once

#include "core/common/macros.h"


namespace gs::detail {

int RegisterCallbacksImpl(void(*types)(), void(*classes)());
template <typename T> inline int RegisterCallbacks(void(*types)(), void(*classes)()) {
	static const int s = [&]() {
		return RegisterCallbacksImpl(types, classes);
	}();

	return s;
}

}

template <typename T> extern void GSAutoRegisterTypesFuncT();
template <typename T> extern void GSAutoRegisterClassesFuncT();

#define DEFINE_DECL(cls)                                \
	template <> void GSAutoRegisterTypesFuncT<cls>();   \
	template <> void GSAutoRegisterClassesFuncT<cls>(); \
	static const int UNIQUE_VAR(auto_register__) =      \
		gs::detail::RegisterCallbacks<cls>(&GSAutoRegisterTypesFuncT<cls>, &GSAutoRegisterClassesFuncT<cls>)
