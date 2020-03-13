#include <string>
#include <vector>
#include <utility>

#include "gtest.h"
#include "core/material/microshader_types.h"

namespace {
using namespace msh;

TEST(Items, Generate) {
    std::vector<Item> data = {"a", "b", "c"};
    Items items;
    items.SetData(std::move(data));

    std::string out;
    items.Generate([](const Item& item, std::string& out){ out.append(item + ","); }, out);

    ASSERT_EQ("a,b,c,", out);
}

}
