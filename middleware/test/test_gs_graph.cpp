#include "gtest.h"

#include <string>
#include <cstdint>
#include <exception>

#include "middleware/gscheme/rttr/variant.h"
#include "middleware/gscheme/graph/gs_graph.h"


namespace {

TEST(gsGraph, SmokeTest) {
    try {
        gs::Graph graph(16);

        uint16_t nodeConstantId1 = graph.AddNode("Constant");
        uint16_t nodeConstantId2 =graph.AddNode("Constant");
        uint16_t nodeSumId = graph.AddNode("Sum");

        graph.AddLink(nodeConstantId1, 0, nodeSumId, 0);
        graph.AddLink(nodeConstantId2, 0, nodeSumId, 1);

        graph.UpdateState();
        graph.ResetChangeState();

        rttr::variant sumResult = graph.GetOutputValue(nodeSumId, 0);
        ASSERT_TRUE(sumResult.is_valid());
        ASSERT_TRUE(sumResult.is_type<float>());
        ASSERT_FLOAT_EQ(0, sumResult.get_value<float>());
    } catch(const std::exception& e) {
        std::string err = e.what();
        ASSERT_EQ(std::string("~!@#$%^&*()_+"), err);
    }
}

}
