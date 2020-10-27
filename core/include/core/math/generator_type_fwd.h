#pragma once


namespace math {

template <typename, typename> class Generator2;
template <typename, typename> class Generator3;

using Generator2f = Generator2<float, void>;
using Generator2d = Generator2<double, void>;

using Generator3f = Generator3<float, void>;
using Generator3d = Generator3<double, void>;

}
