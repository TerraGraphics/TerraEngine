#pragma once

#define CAT_IMPL_(a, b) a##b
#define CAT_(a, b) CAT_IMPL_(a, b)

#ifdef __COUNTER__
#  define UNIQUE_VAR(str) CAT_(str, CAT_(__COUNTER__, CAT_(__LINE__, __COUNTER__)))
#else
#  define UNIQUE_VAR(str) CAT_(str, __LINE__)
#endif
