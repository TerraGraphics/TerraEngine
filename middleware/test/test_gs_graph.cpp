#include "gtest.h"

#include <string>
#include <cstdint>

#include "middleware/gscheme/rttr/variant.h"
#include "middleware/gscheme/graph/gs_graph.h"


namespace {

TEST(gsGraph, SmokeTest) {
    gs::Graph graph(16);

    uint16_t nodeConstantId1 = graph.AddNode("Constant");
    uint16_t nodeConstantId2 =graph.AddNode("Constant");
    uint16_t nodeSumId = graph.AddNode("Sum");

    // without links
    graph.UpdateState();
    graph.ResetChangeState();

    rttr::variant sumResult = graph.GetOutputValue(nodeSumId, 0);
    ASSERT_TRUE(sumResult.is_valid());
    ASSERT_TRUE(sumResult.is_type<float>());
    ASSERT_FLOAT_EQ(0, sumResult.get_value<float>());

    // links with default nodes values
    graph.AddLink(nodeConstantId1, 0, nodeSumId, 0);
    graph.AddLink(nodeConstantId2, 0, nodeSumId, 1);

    graph.UpdateState();
    graph.ResetChangeState();

    sumResult = graph.GetOutputValue(nodeSumId, 0);
    ASSERT_TRUE(sumResult.is_valid());
    ASSERT_TRUE(sumResult.is_type<float>());
    ASSERT_FLOAT_EQ(0, sumResult.get_value<float>());

    // change node - Constant 1
    graph.SetEmbeddedValue(nodeConstantId1, 0, 1.f);
    graph.UpdateState();
    graph.ResetChangeState();

    sumResult = graph.GetOutputValue(nodeSumId, 0);
    ASSERT_TRUE(sumResult.is_valid());
    ASSERT_TRUE(sumResult.is_type<float>());
    ASSERT_FLOAT_EQ(1.f, sumResult.get_value<float>());

    // change node - Constant 2
    graph.SetEmbeddedValue(nodeConstantId2, 0, 2.f);
    graph.UpdateState();
    graph.ResetChangeState();

    sumResult = graph.GetOutputValue(nodeSumId, 0);
    ASSERT_TRUE(sumResult.is_valid());
    ASSERT_TRUE(sumResult.is_type<float>());
    ASSERT_FLOAT_EQ(3.f, sumResult.get_value<float>());
}

TEST(gsGraph, AcyclicityForOneNode) {
    gs::Graph graph(16);

    uint16_t nodeSumId1 = graph.AddNode("Sum");
    ASSERT_ANY_THROW(graph.AddLink(nodeSumId1, 0, nodeSumId1, 0));
}

TEST(gsGraph, AcyclicityForTwoNodes) {
    gs::Graph graph(16);

    uint16_t nodeSumId1 = graph.AddNode("Sum");
    uint16_t nodeSumId2 = graph.AddNode("Sum");
    graph.AddLink(nodeSumId2, 0, nodeSumId1, 0);
    ASSERT_ANY_THROW(graph.AddLink(nodeSumId1, 0, nodeSumId2, 0));
}

TEST(gsGraph, AcyclicityForThreeNodes) {
    gs::Graph graph(16);

    uint16_t nodeSumId1 = graph.AddNode("Sum");
    uint16_t nodeSumId2 = graph.AddNode("Sum");
    uint16_t nodeSumId3 = graph.AddNode("Sum");
    graph.AddLink(nodeSumId2, 0, nodeSumId3, 0);
    graph.AddLink(nodeSumId3, 0, nodeSumId1, 0);
    ASSERT_ANY_THROW(graph.AddLink(nodeSumId1, 0, nodeSumId2, 0));
}

}
