#pragma once

#include <cstdint>

#include "core/common/ctor.h"


namespace gs {

class Node;
class TypeClass;
class Graph : Fixed {
public:
    Graph(uint16_t initialNodeCount = 16);
    ~Graph();

    void ResetChangeState() noexcept;
    void UpdateState();

    Node& AddNode(uint16_t typeClassIndex);

    bool TestRemoveNode(uint16_t nodeId) const noexcept;
    void RemoveNode(uint16_t nodeId);

    bool TestAddLink(uint32_t srcPinId, uint32_t dstPinId) const noexcept;
    uint64_t AddLink(uint32_t srcPinId, uint32_t dstPinId);

    bool TestRemoveLink(uint64_t linkId) const noexcept;
    void RemoveLink(uint64_t linkId);

private:
    void SortNodesByDependency();

    void CheckIsValidNodeId(uint16_t nodeId) const;
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
};

}
