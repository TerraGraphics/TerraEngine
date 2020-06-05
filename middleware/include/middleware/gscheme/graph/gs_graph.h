#pragma once

#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"


namespace rttr {
    class variant;
}
namespace gs {

class Node;
class IDraw;
class TypeClass;
class Graph : Fixed {
public:
    Graph(uint16_t initialNodeCount = 16);
    ~Graph();

    void UpdateState();
    void DrawGraph(IDraw* drawer);
    void DrawNodeProperty(uint16_t nodeId, IDraw* drawer);

    uint16_t CountNodes() const noexcept { return m_capacity - m_free; }

    const TypeClass* TypeClassesBegin() const noexcept;
    const TypeClass* TypeClassesEnd() const noexcept;

    const rttr::variant& GetOutputValue(uint32_t pinId) const;
    const rttr::variant& GetOutputValue(uint16_t nodeId, uint8_t outputPinOffset) const;
    void SetEmbeddedValue(uint32_t pinId, const rttr::variant& value);
    void SetEmbeddedValue(uint16_t nodeId, uint8_t embeddedPinOffset, const rttr::variant& value);
    void SetInputValue(uint32_t pinId, const rttr::variant& value);
    void SetInputValue(uint16_t nodeId, uint8_t inputPinOffset, const rttr::variant& value);

    uint16_t AddNode(uint16_t typeClassIndex);
    uint16_t AddNode(std::string_view name);

    bool TestRemoveNode(uint16_t nodeId) const noexcept;
    void RemoveNode(uint16_t nodeId);

    bool TestAddLink(uint32_t srcPinId, uint32_t dstPinId) const noexcept;
    uint64_t AddLink(uint32_t srcPinId, uint32_t dstPinId);
    uint64_t AddLink(uint16_t srcNodeId, uint8_t outputPinOffset, uint16_t dstNodeId, uint8_t inputPinOffset);

    bool TestRemoveLink(uint64_t linkId) const noexcept;
    void RemoveLink(uint64_t linkId);

private:
    void SortNodesByDependency();

    void CheckIsValidNodeId(uint16_t nodeId) const;
    void CheckIsValidEmbededPinId(uint32_t pinId) const;
    void CheckIsValidInputPinId(uint32_t pinId) const;
    void CheckIsValidOutputPinId(uint32_t pinId) const;

    void CheckRemoveNode(uint16_t nodeId) const;
    void CheckAddLink(uint32_t srcPinId, uint32_t dstPinId) const;
    void CheckRemoveLink(uint64_t linkId) const;

private:
    uint16_t m_free = 0;
    uint16_t m_capacity = 0;
    uint16_t m_firstFreeIndex = 0;
    uint16_t m_firstCalcIndex = 0;
    uint16_t m_countTypeClasses = 0;
    // TODO: add binary search
    TypeClass* m_typeClasses;
    Node* m_nodes = nullptr;
    uint16_t* m_indeciesForOrder = nullptr;
};

}
