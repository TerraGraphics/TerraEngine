#pragma once

#define CAT_IMPL_(a, b) a##b
#define CAT_(a, b) CAT_IMPL_(a, b)

#ifdef __COUNTER__
#  define UNIQ_VAR(str) CAT_(str, CAT_(__COUNTER__, CAT_(__LINE__, __COUNTER__)))
#else
#  define UNIQ_VAR(str) CAT_(str, __LINE__)
#endif

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
	static const int UNIQ_VAR(auto_register__) =                                  \
		detail::GetReg<cls>(&rttr_auto_register_reflection_function_t<cls>)

#define REFLECTION_CLASS_DECL(cls) class cls; REFLECTION_DECL(cls)