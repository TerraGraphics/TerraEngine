#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wimplicit-int-float-conversion"
#include <rttr/registration.h>
#pragma GCC diagnostic pop

#define REFLECTION_IMPL(cls) template <> void rttr_auto_register_reflection_function_t<cls>()
