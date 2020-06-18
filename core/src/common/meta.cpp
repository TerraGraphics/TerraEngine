#include "core/common/meta.h"

#include <cstdlib>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif

namespace meta {

const std::string DemangleTypeName(const char* name) {
#ifndef _MSC_VER
    int status = -4;
    char* res = abi::__cxa_demangle(name, NULL, NULL, &status);
    const char* const demangledName = (status == 0) ? res : name;
    std::string result(demangledName);
    free(res);

    return result;
#else
    return std::string(name);
#endif
}

}