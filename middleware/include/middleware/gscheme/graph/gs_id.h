#pragma once

#include <cstdint>

/*
    valid ID (pinID, linkID, nodeID) more than 0
    uint16_t nodeID - index in Graph::m_nodes + 1
    uint8_t pinIndex - index in Node::m_pins
    bool isInput - input or output pin
    bool isEmbeded - formally input pin, without the ability to connect a link to it, if this bit is set, the isInput bit is not parsed
    uint32_t pinID = nodeID << 16 | pinIndex << 8 | isEmbeded << 1 | isInput
    uint64_t linkID = srcPinID << 32 | dstPinID == outputPinID << 32 | inputPinID
*/

namespace gs {

inline uint16_t NodeIdFromPinId(uint32_t pinId) {
    return static_cast<uint16_t>(pinId >> uint32_t(16));
}

inline uint16_t NodeIndexFromPinId(uint32_t pinId) {
    return static_cast<uint16_t>(pinId >> uint32_t(16)) - 1;
}

inline uint8_t PinIndexFromPinId(uint32_t pinId) {
    return static_cast<uint8_t>((pinId >> uint32_t(8)) & uint32_t(0xFF));
}

inline bool IsEmbededFromPinId(uint32_t pinId) {
    return ((pinId >> uint32_t(1)) & uint32_t(1));
}

inline bool IsInputFromPinId(uint32_t pinId) {
    return ((pinId & uint32_t(1)) == 1);
}

inline uint32_t SrcPinIdFromLinkId(uint64_t linkId) {
    return static_cast<uint32_t>(linkId >> uint64_t(32));
}

inline uint32_t DstPinIdFromLinkId(uint64_t linkId) {
    return static_cast<uint32_t>(linkId & uint64_t(0xFFFFFFFF));
}

inline uint64_t LinkId(uint32_t srcPinId, uint32_t dstPinId) {
    return (static_cast<uint64_t>(srcPinId) << uint64_t(32)) | static_cast<uint64_t>(dstPinId);
}

}
