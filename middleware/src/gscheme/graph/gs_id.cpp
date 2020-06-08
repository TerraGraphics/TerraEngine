#include "middleware/gscheme/graph/gs_id.h"

#include <string>

#include "rttr/rttr.h"
#include "eigen/core.h"


namespace gs {

uintptr_t TypeIdFloat() {
    static const rttr::type::type_id typeId = rttr::type::get<float>().get_id();
    return typeId;
}

uintptr_t TypeIdVector2f() {
    static const rttr::type::type_id typeId = rttr::type::get<Eigen::Vector2f>().get_id();
    return typeId;
}

uintptr_t TypeIdVector3f() {
    static const rttr::type::type_id typeId = rttr::type::get<Eigen::Vector3f>().get_id();
    return typeId;
}

uintptr_t TypeIdVector4f() {
    static const rttr::type::type_id typeId = rttr::type::get<Eigen::Vector4f>().get_id();
    return typeId;
}

uintptr_t TypeIdString() {
    static const rttr::type::type_id typeId = rttr::type::get<std::string>().get_id();
    return typeId;
}

bool IsValidPinTypeId(uintptr_t typeId) {
    return (
        (typeId == TypeIdFloat()) ||
        (typeId == TypeIdVector2f()) ||
        (typeId == TypeIdVector3f()) ||
        (typeId == TypeIdVector4f())
    );
}

}
