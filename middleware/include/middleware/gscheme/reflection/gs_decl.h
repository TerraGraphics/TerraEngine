#pragma once

#include "core/common/defines.h"


namespace detail {

template <typename T> inline int GetReg(void (*f)()) {
	static const int s = [&f]() {
		f();
		return 0;
	}();
	return s;
}

}

#define REFLECTION_DECL(cls)                                                      \
	template <typename T> extern void rttr_auto_register_reflection_function_t(); \
	template <> void rttr_auto_register_reflection_function_t<cls>();             \
	static const int UNIQUE_VAR(auto_register__) =                                  \
		detail::GetReg<cls>(&rttr_auto_register_reflection_function_t<cls>)

#define REFLECTION_CLASS_DECL(cls) class cls; REFLECTION_DECL(cls)
