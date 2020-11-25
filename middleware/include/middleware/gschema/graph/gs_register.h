#pragma once

#include "core/common/macros.h"


namespace gs {
namespace detail {

template <typename T> inline int GetReg(void (*f)()) {
	static const int s = [&f]() {
		f();
		return 0;
	}();
	return s;
}

}
}

#define REFLECTION_DECL(cls)                                                      \
	template <typename T> extern void gs_auto_register_reflection_function_t(); \
	template <> void gs_auto_register_reflection_function_t<cls>();             \
	static const int UNIQUE_VAR(auto_register__) =                                \
		gs::detail::GetReg<cls>(&gs_auto_register_reflection_function_t<cls>)


#define REFLECTION_IMPL(cls) template <> void gs_auto_register_reflection_function_t<cls>()
