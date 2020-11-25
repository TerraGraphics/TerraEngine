#include <string>
#include <variant>

#include "test/test.h"
#include "eigen/core.h"
#include "middleware/gschema/embedded/func_add.h"


namespace {

TEST(GSFuncs, FuncAdd) {
    gs::FuncAdd f;
    f.SetA(1.f);
    f.SetB(2.f);

    ASSERT_FLOAT_EQ(1.f, std::get<float>(f.GetA()));
    ASSERT_FLOAT_EQ(2.f, std::get<float>(f.GetB()));
    ASSERT_FLOAT_EQ(3.f, std::get<float>(f.Result()));

    f.SetA(Eigen::Vector2f(3.f, 4.f));
    ASSERT_EQ(Eigen::Vector2f(3.f, 4.f), std::get<Eigen::Vector2f>(f.GetA()));
    ASSERT_EQ(Eigen::Vector2f(5.f, 6.f), std::get<Eigen::Vector2f>(f.Result()));
}

}
