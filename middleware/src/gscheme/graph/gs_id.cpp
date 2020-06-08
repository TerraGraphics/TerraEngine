#include "middleware/gscheme/graph/gs_id.h"

#include <string>

#include "rttr/rttr.h"
#include "eigen/core.h"
#include "core/math/generator_type.h"


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

uintptr_t TypeIdGenerator2D() {
    static const rttr::type::type_id typeId = rttr::type::get<math::Generator2D>().get_id();
    return typeId;
}

uintptr_t TypeIdGenerator3D() {
    static const rttr::type::type_id typeId = rttr::type::get<math::Generator3D>().get_id();
    return typeId;
}

uintptr_t TypeIdString() {
    static const rttr::type::type_id typeId = rttr::type::get<std::string>().get_id();
    return typeId;
}

bool IsValidEmbeddedPinTypeId(uintptr_t typeId) {
    return (
        (typeId == TypeIdFloat()) ||
        (typeId == TypeIdVector2f()) ||
        (typeId == TypeIdVector3f()) ||
        (typeId == TypeIdVector4f()) ||
        (typeId == TypeIdGenerator2D()) ||
        (typeId == TypeIdGenerator3D())
    );
}

bool IsValidInputPinTypeId(uintptr_t typeId) {
    return (
        (typeId == TypeIdFloat()) ||
        (typeId == TypeIdVector2f()) ||
        (typeId == TypeIdVector3f()) ||
        (typeId == TypeIdVector4f()) ||
        (typeId == TypeIdGenerator2D()) ||
        (typeId == TypeIdGenerator3D())
    );
}

bool IsValidOutputPinTypeId(uintptr_t typeId) {
    return (
        (typeId == TypeIdFloat()) ||
        (typeId == TypeIdVector2f()) ||
        (typeId == TypeIdVector3f()) ||
        (typeId == TypeIdVector4f()) ||
        (typeId == TypeIdGenerator2D()) ||
        (typeId == TypeIdGenerator3D())
    );
}

}
