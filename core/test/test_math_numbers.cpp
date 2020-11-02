#include <limits>
#include <memory>
#include <cstdint>

#include "test/test.h"
#include "core/math/numbers.h"


namespace {

class MathNumbers : public ::testing::Test {
};

TEST_F(MathNumbers, ToInt32Continuous) {
    EXPECT_EQ(math::ToInt32Continuous(1.), 1);
    EXPECT_EQ(math::ToInt32Continuous(-1.), -1);
    EXPECT_EQ(math::ToInt32Continuous(0), 0);

    const auto maxi = std::numeric_limits<int32_t>::max();
    const auto mini = std::numeric_limits<int32_t>::min();
    const auto maxf = static_cast<double>(maxi);
    const auto minf = static_cast<double>(mini);
    const auto coutf = maxf - minf + 1.;

    EXPECT_EQ(math::ToInt32Continuous(maxf      ), maxi);
    EXPECT_EQ(math::ToInt32Continuous(maxf +  1.), mini);
    EXPECT_EQ(math::ToInt32Continuous(maxf +  2.), mini + 1);
    EXPECT_EQ(math::ToInt32Continuous(maxf + 20.), mini + 19);

    EXPECT_EQ(math::ToInt32Continuous(maxf + coutf*3.      ), maxi);
    EXPECT_EQ(math::ToInt32Continuous(maxf + coutf*3.  + 1.), mini);
    EXPECT_EQ(math::ToInt32Continuous(maxf + coutf*3.  + 2.), mini + 1);
    EXPECT_EQ(math::ToInt32Continuous(maxf + coutf*3. + 20.), mini + 19);

    EXPECT_EQ(math::ToInt32Continuous(minf      ), mini);
    EXPECT_EQ(math::ToInt32Continuous(minf -  1.), maxi);
    EXPECT_EQ(math::ToInt32Continuous(minf -  2.), maxi - 1);
    EXPECT_EQ(math::ToInt32Continuous(minf - 20.), maxi - 19);

    EXPECT_EQ(math::ToInt32Continuous(minf + coutf*3      ), mini);
    EXPECT_EQ(math::ToInt32Continuous(minf + coutf*3 -  1.), maxi);
    EXPECT_EQ(math::ToInt32Continuous(minf + coutf*3 -  2.), maxi - 1);
    EXPECT_EQ(math::ToInt32Continuous(minf + coutf*3 - 20.), maxi - 19);
}

}
