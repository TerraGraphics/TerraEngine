#pragma once

#include <memory>
#include <cstdint>
#include <string_view>

#include "cpgf/variant.h"
#include "core/common/ctor.h"
#include "middleware/gscheme/graph/gs_types.h"
#include "middleware/gscheme/graph/gs_types_decl.h"


namespace gs {

class Node;
class IDraw;
class ClassStorage;
class Graph : Fixed {
public:
    Graph() = delete;
    Graph(const std::shared_ptr<ClassStorage>& classStorage, uint16_t initialNodeCount = 16);
    ~Graph();

    void UpdateState();
    void DrawGraph(IDraw* drawer);
    void DrawNodeProperty(uint16_t nodeId, IDraw* drawer);

    uint16_t CountNodes() const noexcept { return m_capacity - m_free; }

    const cpgf::GVariant& GetOutputValue(uint32_t pinId) const;
    const cpgf::GVariant& GetOutputValue(uint16_t nodeId, uint8_t outputPinOffset) const;

    template<typename T, typename Enable = std::enable_if_t<IsEmbedded<T>>>
        void SetEmbeddedValue(uint32_t pinId, const T& value) {
            SetEmbeddedValueImpl(pinId, cpgf::createVariant<T>(value, true), GetTypeId<T>());
        }
    template<typename T, typename Enable = std::enable_if_t<IsEmbedded<T>>>
        void SetEmbeddedValue(uint16_t nodeId, uint8_t embeddedPinOffset, const T& value) {
            SetEmbeddedValueImpl(nodeId, embeddedPinOffset, cpgf::createVariant<T>(value, true), GetTypeId<T>());
        }
    template<typename T, typename Enable = std::enable_if_t<IsInput<T>>>
        void SetInputValue(uint32_t pinId, const T& value) {
            SetInputValueImpl(pinId, cpgf::createVariant<T>(value, true), GetTypeId<T>());
        }
    template<typename T, typename Enable = std::enable_if_t<IsInput<T>>>
        void SetInputValue(uint16_t nodeId, uint8_t inputPinOffset, const T& value) {
            SetInputValueImpl(nodeId, inputPinOffset, cpgf::createVariant<T>(value, true), GetTypeId<T>());
        }

    uint16_t AddNode(uint16_t classIndex);
    uint16_t AddNode(std::string_view name);

    bool TestRemoveNode(uint16_t nodeId) const noexcept;
    void RemoveNode(uint16_t nodeId);

    bool TestAddLink(uint32_t srcPinId, uint32_t dstPinId) const noexcept;
    uint64_t AddLink(uint32_t srcPinId, uint32_t dstPinId);
    uint64_t AddLink(uint16_t srcNodeId, uint8_t outputPinOffset, uint16_t dstNodeId, uint8_t inputPinOffset);

    bool TestRemoveLink(uint64_t linkId) const noexcept;
    void RemoveLink(uint64_t linkId);

private:
    void SetEmbeddedValueImpl(uint32_t pinId, const cpgf::GVariant& value, TypeId typeId);
    void SetEmbeddedValueImpl(uint16_t nodeId, uint8_t embeddedPinOffset, const cpgf::GVariant& value, TypeId typeId);
    void SetInputValueImpl(uint32_t pinId, const cpgf::GVariant& value, TypeId typeId);
    void SetInputValueImpl(uint16_t nodeId, uint8_t inputPinOffset, const cpgf::GVariant& value, TypeId typeId);

    void SortNodesByDependency();

    void CheckIsValidNodeId(uint16_t nodeId) const;
    void CheckIsValidEmbeddedPinId(uint32_t pinId) const;
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
    Node* m_nodes = nullptr;
    uint16_t* m_indeciesForOrder = nullptr;
    std::shared_ptr<ClassStorage> m_classStorage;
};

}
