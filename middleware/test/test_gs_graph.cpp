#include <memory>
#include <cstdint>
#include <variant>

#include "test/test.h"
#include "eigen/core.h"
#include "cpgf/variant.h"
#include "middleware/gschema/graph/gs_types.h"
#include "middleware/gschema/graph/gs_graph.h"
#include "middleware/gschema/graph/gs_class_storage.h"


#define ASSERT_VARIANT_FLOAT(expected, actual) do { \
    const auto& tmpFloatValue = actual; \
    ASSERT_FALSE(tmpFloatValue.isEmpty()); \
    ASSERT_TRUE(cpgf::canFromVariant<gs::UniversalType>(tmpFloatValue)); \
    ASSERT_FLOAT_EQ(expected, std::get<float>(cpgf::fromVariant<gs::UniversalType>(tmpFloatValue))); \
    } while(false)

#define ASSERT_VARIANT_VECTOR2F(expected, actual) do { \
    const auto& tmpVec2fValue = actual; \
    ASSERT_FALSE(tmpVec2fValue.isEmpty()); \
    ASSERT_TRUE(cpgf::canFromVariant<gs::UniversalType>(tmpVec2fValue)); \
    ASSERT_EQ(expected, std::get<Eigen::Vector2f>(cpgf::fromVariant<gs::UniversalType>(tmpVec2fValue))); \
    } while(false)

namespace {

class GSGraphSuite : public ::testing::Test {
protected:
    GSGraphSuite() = default;
    ~GSGraphSuite() = default;

    void SetUp() final {
        m_classStorage = std::make_shared<gs::ClassStorage>();
    }

    std::shared_ptr<gs::ClassStorage> m_classStorage;
};

TEST_F(GSGraphSuite, DoubleRemoveNode) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeId = graph.AddNode("Constant");
    ASSERT_EQ(1, graph.CountNodes());

    ASSERT_TRUE(graph.TestRemoveNode(nodeId));
    graph.RemoveNode(nodeId);
    ASSERT_EQ(0, graph.CountNodes());

    ASSERT_FALSE(graph.TestRemoveNode(nodeId));
    ASSERT_ANY_THROW(graph.RemoveNode(nodeId));
    ASSERT_EQ(0, graph.CountNodes());

    ASSERT_FALSE(graph.TestRemoveNode(10000));
    ASSERT_ANY_THROW(graph.RemoveNode(10000));
    ASSERT_EQ(0, graph.CountNodes());
}

TEST_F(GSGraphSuite, DoubleAddLink) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    uint16_t addId = graph.AddNode("Add");

    ASSERT_TRUE(graph.TestAddLink(constantId, 0, addId, 0));
    graph.AddLink(constantId, 0, addId, 0);

    ASSERT_FALSE(graph.TestAddLink(constantId, 0, addId, 0));
    ASSERT_ANY_THROW(graph.AddLink(constantId, 0, addId, 0));
}

TEST_F(GSGraphSuite, DoubleRemoveLink) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    uint16_t addId = graph.AddNode("Add");
    uint64_t linkId = graph.AddLink(constantId, 0, addId, 0);

    ASSERT_TRUE(graph.TestRemoveLink(linkId));
    graph.RemoveLink(linkId);

    ASSERT_FALSE(graph.TestRemoveLink(linkId));
    ASSERT_ANY_THROW(graph.RemoveLink(linkId));

    ASSERT_FALSE(graph.TestRemoveLink(100000));
    ASSERT_ANY_THROW(graph.RemoveLink(100000));
}

TEST_F(GSGraphSuite, ReplaceLink) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t constantId1 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(constantId1, 0, 1.f);

    uint16_t constantId2 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(constantId2, 0, 2.f);

    uint16_t addId = graph.AddNode("Add");
    ASSERT_TRUE(graph.TestAddLink(constantId1, 0, addId, 0));
    graph.AddLink(constantId1, 0, addId, 0);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(1.f, graph.GetOutputValue(addId, 0));

    ASSERT_TRUE(graph.TestAddLink(constantId2, 0, addId, 0));
    graph.AddLink(constantId2, 0, addId, 0);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(2.f, graph.GetOutputValue(addId, 0));
}

TEST_F(GSGraphSuite, ChangeEmbeddedPins) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeId = graph.AddNode("Constant");

    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(0, graph.GetOutputValue(nodeId, 0));

    graph.SetEmbeddedValue(nodeId, 0, 1.f);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(1.f, graph.GetOutputValue(nodeId, 0));
}

TEST_F(GSGraphSuite, SetInvalidTypeForEmbeddedPins) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeId = graph.AddNode("Constant");
    ASSERT_ANY_THROW(graph.SetEmbeddedValue(nodeId, 0, uint32_t(1)));
    ASSERT_ANY_THROW(graph.SetEmbeddedValue(nodeId, 0, Eigen::Vector2f(1.f, 2.f)));
}

TEST_F(GSGraphSuite, ChangeInvalidEmbeddedPins) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    ASSERT_ANY_THROW(graph.SetEmbeddedValue(constantId, 10, 1.f));

    uint16_t nodeAddId = graph.AddNode("Add");
    ASSERT_ANY_THROW(graph.SetEmbeddedValue(nodeAddId, 0, 1.f));
}

TEST_F(GSGraphSuite, ChangeInputPins) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeAddId = graph.AddNode("Add");

    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(0, graph.GetOutputValue(nodeAddId, 0));

    graph.SetInputValue(nodeAddId, 0, 1.f);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(1.f, graph.GetOutputValue(nodeAddId, 0));

    graph.SetInputValue(nodeAddId, 1, 2.f);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(3.f, graph.GetOutputValue(nodeAddId, 0));

    uint16_t nodeConstantId1 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(nodeConstantId1, 0, 10.f);
    uint64_t link1 = graph.AddLink(nodeConstantId1, 0, nodeAddId, 0);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(12.f, graph.GetOutputValue(nodeAddId, 0));

    uint16_t nodeConstantId2 = graph.AddNode("Constant");
    graph.SetEmbeddedValue(nodeConstantId2, 0, 20.f);
    uint64_t link2 = graph.AddLink(nodeConstantId2, 0, nodeAddId, 1);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(30.f, graph.GetOutputValue(nodeAddId, 0));

    graph.RemoveLink(link2);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(10.f, graph.GetOutputValue(nodeAddId, 0));

    graph.RemoveLink(link1);
    graph.UpdateState();
    ASSERT_VARIANT_FLOAT(0.f, graph.GetOutputValue(nodeAddId, 0));
}

TEST_F(GSGraphSuite, HeterogeneousGraph) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t constantId1 = graph.AddNode("Constant");
    uint16_t constantId2 = graph.AddNode("Constant2");
    uint16_t constantId3 = graph.AddNode("Constant3");
    uint16_t addId1 = graph.AddNode("Add");
    uint16_t addId2 = graph.AddNode("Add");

    graph.AddLink(constantId1, 0, addId1, 0);
    graph.UpdateState();

    graph.AddLink(constantId2, 0, addId1, 1);
    graph.UpdateState();

    graph.AddLink(addId1, 0, addId2, 0);
    graph.UpdateState();

    ASSERT_VARIANT_VECTOR2F(Eigen::Vector2f(0, 0), graph.GetOutputValue(addId1, 0));
    ASSERT_VARIANT_VECTOR2F(Eigen::Vector2f(0, 0), graph.GetOutputValue(addId2, 0));

    graph.SetEmbeddedValue(constantId1, 0, 1.f);
    graph.SetEmbeddedValue(constantId2, 0, Eigen::Vector2f(2.f, 3.f));
    graph.SetEmbeddedValue(constantId3, 0, Eigen::Vector3f(4.f, 5.f, 6.f));
    graph.UpdateState();

    ASSERT_VARIANT_VECTOR2F(Eigen::Vector2f(3.f, 4.f), graph.GetOutputValue(addId1, 0));
    ASSERT_VARIANT_VECTOR2F(Eigen::Vector2f(3.f, 4.f), graph.GetOutputValue(addId2, 0));

    // Invalid link
    graph.AddLink(constantId3, 0, addId1, 0);
    graph.UpdateState();

    // Valid ling
    graph.AddLink(constantId1, 0, addId2, 1);
    graph.UpdateState();

    // value from cache
    ASSERT_VARIANT_VECTOR2F(Eigen::Vector2f(3.f, 4.f), graph.GetOutputValue(addId1, 0));
    // cache + value from new link
    ASSERT_VARIANT_VECTOR2F(Eigen::Vector2f(4.f, 5.f), graph.GetOutputValue(addId2, 0));
}

TEST_F(GSGraphSuite, SetInvalidTypeForInputPins) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeAddId = graph.AddNode("Add");

    ASSERT_ANY_THROW(graph.SetInputValue(nodeAddId, 0, Eigen::Vector2f(1.f, 2.f)));
}

TEST_F(GSGraphSuite, ChangeInvalidInputPins) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    ASSERT_ANY_THROW(graph.SetInputValue(constantId, 0, 1.f));

    uint16_t nodeAddId = graph.AddNode("Add");
    ASSERT_ANY_THROW(graph.SetInputValue(nodeAddId, 10, 1.f));
}

TEST_F(GSGraphSuite, GetInvalidOutputPins) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t constantId = graph.AddNode("Constant");
    ASSERT_ANY_THROW(graph.GetOutputValue(constantId, 10));
}

TEST_F(GSGraphSuite, AcyclicityForOneNode) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeAddId1 = graph.AddNode("Add");
    ASSERT_ANY_THROW(graph.AddLink(nodeAddId1, 0, nodeAddId1, 0));
}

TEST_F(GSGraphSuite, AcyclicityForTwoNodes) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeAddId1 = graph.AddNode("Add");
    uint16_t nodeAddId2 = graph.AddNode("Add");
    graph.AddLink(nodeAddId2, 0, nodeAddId1, 0);
    ASSERT_ANY_THROW(graph.AddLink(nodeAddId1, 0, nodeAddId2, 0));
}

TEST_F(GSGraphSuite, AcyclicityForThreeNodes) {
    gs::Graph graph(m_classStorage, 16);

    uint16_t nodeAddId1 = graph.AddNode("Add");
    uint16_t nodeAddId2 = graph.AddNode("Add");
    uint16_t nodeAddId3 = graph.AddNode("Add");
    graph.AddLink(nodeAddId2, 0, nodeAddId3, 0);
    graph.AddLink(nodeAddId3, 0, nodeAddId1, 0);
    ASSERT_ANY_THROW(graph.AddLink(nodeAddId1, 0, nodeAddId2, 0));
}

}
