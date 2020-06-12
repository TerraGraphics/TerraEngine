#include <string>
#include <memory>
#include <cstdint>

#include "test/test.h"
#include "rttr/rttr.h"
#include "middleware/gscheme/graph/gs_graph.h"
#include "middleware/gscheme/graph/gs_type_storage.h"


#define ASSERT_FLOAT(expected, actual) do { \
    const auto& tmpFloatValue = actual; \
    ASSERT_TRUE(tmpFloatValue.is_valid()); \
    ASSERT_TRUE(tmpFloatValue.is_type<float>()); \
    ASSERT_FLOAT_EQ(expected, tmpFloatValue.get_value<float>()); \
    } while(false)

namespace {

class GSGraphSuite : public ::testing::Test {
protected:
    GSGraphSuite() = default;
    ~GSGraphSuite() = default;

    void SetUp() final {
        m_typeStorage = std::make_shared<gs::TypeStorage>();
    }

    std::shared_ptr<gs::TypeStorage> m_typeStorage;
};

TEST_F(GSGraphSuite, DoubleRemoveNode) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t nodeId = graph.AddNode("Constant");
    graph.RemoveNode(nodeId);
    ASSERT_EQ(0, graph.CountNodes());

    ASSERT_ANY_THROW(graph.RemoveNode(nodeId));
    ASSERT_ANY_THROW(graph.RemoveNode(10000));
}

TEST_F(GSGraphSuite, DoubleAddLink) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    uint16_t sumId = graph.AddNode("Sum");
    graph.AddLink(constantId, 0, sumId, 0);
    ASSERT_ANY_THROW(graph.AddLink(constantId, 0, sumId, 0));
}

TEST_F(GSGraphSuite, DoubleRemoveLink) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    uint16_t sumId = graph.AddNode("Sum");
    uint64_t linkId = graph.AddLink(constantId, 0, sumId, 0);

    graph.RemoveLink(linkId);
    ASSERT_ANY_THROW(graph.RemoveLink(linkId));
    ASSERT_ANY_THROW(graph.RemoveLink(100000));
}

TEST_F(GSGraphSuite, ReplaceLink) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t constantId1 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(constantId1, 0, 1.f);

    uint16_t constantId2 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(constantId2, 0, 2.f);

    uint16_t sumId = graph.AddNode("Sum");
    graph.AddLink(constantId1, 0, sumId, 0);
    graph.UpdateState();
    ASSERT_FLOAT(1.f, graph.GetOutputValue(sumId, 0));

    graph.AddLink(constantId2, 0, sumId, 0);
    graph.UpdateState();
    ASSERT_FLOAT(2.f, graph.GetOutputValue(sumId, 0));
}

TEST_F(GSGraphSuite, ChangeEmbeddedPins) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t nodeId = graph.AddNode("Constant");

    graph.UpdateState();
    ASSERT_FLOAT(0, graph.GetOutputValue(nodeId, 0));

    graph.SetEmbeddedValue(nodeId, 0, 1.f);
    graph.UpdateState();
    ASSERT_FLOAT(1.f, graph.GetOutputValue(nodeId, 0));
}

TEST_F(GSGraphSuite, ChangeInvalidEmbeddedPins) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    ASSERT_ANY_THROW(graph.SetEmbeddedValue(constantId, 10, 1.f));

    uint16_t nodeSumId = graph.AddNode("Sum");
    ASSERT_ANY_THROW(graph.SetEmbeddedValue(nodeSumId, 0, 1.f));
}

TEST_F(GSGraphSuite, ChangeInputPins) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t nodeSumId = graph.AddNode("Sum");

    graph.UpdateState();
    ASSERT_FLOAT(0, graph.GetOutputValue(nodeSumId, 0));

    graph.SetInputValue(nodeSumId, 0, 1.f);
    graph.UpdateState();
    ASSERT_FLOAT(1.f, graph.GetOutputValue(nodeSumId, 0));

    graph.SetInputValue(nodeSumId, 1, 2.f);
    graph.UpdateState();
    ASSERT_FLOAT(3.f, graph.GetOutputValue(nodeSumId, 0));

    uint16_t nodeConstantId1 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(nodeConstantId1, 0, 10.f);
    uint64_t link1 = graph.AddLink(nodeConstantId1, 0, nodeSumId, 0);
    graph.UpdateState();
    ASSERT_FLOAT(12.f, graph.GetOutputValue(nodeSumId, 0));

    uint16_t nodeConstantId2 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(nodeConstantId2, 0, 20.f);
    uint64_t link2 = graph.AddLink(nodeConstantId2, 0, nodeSumId, 1);
    graph.UpdateState();
    ASSERT_FLOAT(30.f, graph.GetOutputValue(nodeSumId, 0));

    graph.RemoveLink(link2);
    graph.UpdateState();
    ASSERT_FLOAT(10.f, graph.GetOutputValue(nodeSumId, 0));

    graph.RemoveLink(link1);
    graph.UpdateState();
    ASSERT_FLOAT(0.f, graph.GetOutputValue(nodeSumId, 0));
}

TEST_F(GSGraphSuite, ChangeInvalidInputPins) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    ASSERT_ANY_THROW(graph.SetInputValue(constantId, 0, 1.f));

    uint16_t nodeSumId = graph.AddNode("Sum");
    ASSERT_ANY_THROW(graph.SetInputValue(nodeSumId, 10, 1.f));
}

TEST_F(GSGraphSuite, GetInvalidOutputPins) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    ASSERT_ANY_THROW(graph.GetOutputValue(constantId, 10));
}

TEST_F(GSGraphSuite, AcyclicityForOneNode) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t nodeSumId1 = graph.AddNode("Sum");
    ASSERT_ANY_THROW(graph.AddLink(nodeSumId1, 0, nodeSumId1, 0));
}

TEST_F(GSGraphSuite, AcyclicityForTwoNodes) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t nodeSumId1 = graph.AddNode("Sum");
    uint16_t nodeSumId2 = graph.AddNode("Sum");
    graph.AddLink(nodeSumId2, 0, nodeSumId1, 0);
    ASSERT_ANY_THROW(graph.AddLink(nodeSumId1, 0, nodeSumId2, 0));
}

TEST_F(GSGraphSuite, AcyclicityForThreeNodes) {
    gs::Graph graph(m_typeStorage, 16);

    uint16_t nodeSumId1 = graph.AddNode("Sum");
    uint16_t nodeSumId2 = graph.AddNode("Sum");
    uint16_t nodeSumId3 = graph.AddNode("Sum");
    graph.AddLink(nodeSumId2, 0, nodeSumId3, 0);
    graph.AddLink(nodeSumId3, 0, nodeSumId1, 0);
    ASSERT_ANY_THROW(graph.AddLink(nodeSumId1, 0, nodeSumId2, 0));
}

}
