#include "middleware/gscheme/graph/gs_types.h"

#include "rttr/rttr.h"


namespace gs {

TypeId MaxTypeId(TypeId a, TypeId b) {
    constexpr const uint8_t mask = (static_cast<uint8_t>(TypeId::UniversalType) - uint8_t(1));
    uint8_t aVal = static_cast<uint8_t>(a) & mask;
    uint8_t bVal = static_cast<uint8_t>(b) & mask;

    return static_cast<TypeId>((aVal > bVal) ? aVal : bVal);
}


uintptr_t RttrTypeIdInt8() {
    static const rttr::type::type_id typeId = rttr::type::get<int8_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdUint8() {
    static const rttr::type::type_id typeId = rttr::type::get<uint8_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdInt16() {
    static const rttr::type::type_id typeId = rttr::type::get<int16_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdUint16() {
    static const rttr::type::type_id typeId = rttr::type::get<uint16_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdInt32() {
    static const rttr::type::type_id typeId = rttr::type::get<int32_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdUint32() {
    static const rttr::type::type_id typeId = rttr::type::get<uint32_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdInt64() {
    static const rttr::type::type_id typeId = rttr::type::get<int64_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdUint64() {
    static const rttr::type::type_id typeId = rttr::type::get<uint64_t>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdFloat() {
    static const rttr::type::type_id typeId = rttr::type::get<float>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdDouble() {
    static const rttr::type::type_id typeId = rttr::type::get<double>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdVector2f() {
    static const rttr::type::type_id typeId = rttr::type::get<Eigen::Vector2f>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdVector3f() {
    static const rttr::type::type_id typeId = rttr::type::get<Eigen::Vector3f>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdVector4f() {
    static const rttr::type::type_id typeId = rttr::type::get<Eigen::Vector4f>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdGenerator2D() {
    static const rttr::type::type_id typeId = rttr::type::get<math::Generator2D>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdGenerator3D() {
    static const rttr::type::type_id typeId = rttr::type::get<math::Generator3D>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdString() {
    static const rttr::type::type_id typeId = rttr::type::get<std::string>().get_id();
    return typeId;
}

uintptr_t RttrTypeIdUniversalType() {
    static const rttr::type::type_id typeId = rttr::type::get<UniversalType>().get_id();
    return typeId;
}

bool IsValidEmbeddedPinRttrTypeId(uintptr_t typeId) {
    return (
        (typeId == RttrTypeIdInt8()) ||
        (typeId == RttrTypeIdUint8()) ||
        (typeId == RttrTypeIdInt16()) ||
        (typeId == RttrTypeIdUint16()) ||
        (typeId == RttrTypeIdInt32()) ||
        (typeId == RttrTypeIdUint32()) ||
        (typeId == RttrTypeIdInt64()) ||
        (typeId == RttrTypeIdUint64()) ||
        (typeId == RttrTypeIdFloat()) ||
        (typeId == RttrTypeIdDouble()) ||
        (typeId == RttrTypeIdVector2f()) ||
        (typeId == RttrTypeIdVector3f()) ||
        (typeId == RttrTypeIdVector4f()) ||
        (typeId == RttrTypeIdString())
    );
}

bool IsValidInputPinRttrTypeId(uintptr_t typeId) {
    return (
        (typeId == RttrTypeIdFloat()) ||
        (typeId == RttrTypeIdVector2f()) ||
        (typeId == RttrTypeIdVector3f()) ||
        (typeId == RttrTypeIdVector4f()) ||
        (typeId == RttrTypeIdGenerator2D()) ||
        (typeId == RttrTypeIdGenerator3D()) ||
        (typeId == RttrTypeIdUniversalType())
    );
}

bool IsValidOutputPinRttrTypeId(uintptr_t typeId) {
    return (
        (typeId == RttrTypeIdFloat()) ||
        (typeId == RttrTypeIdVector2f()) ||
        (typeId == RttrTypeIdVector3f()) ||
        (typeId == RttrTypeIdVector4f()) ||
        (typeId == RttrTypeIdGenerator2D()) ||
        (typeId == RttrTypeIdGenerator3D()) ||
        (typeId == RttrTypeIdUniversalType())
    );
}

}
