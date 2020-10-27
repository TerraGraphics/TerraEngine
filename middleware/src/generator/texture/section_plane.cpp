#include "middleware/generator/texture/section_plane.h"


math::Generator2D SectionPlaneX0Y::Result() const {
    return math::Generator2D([input = m_input, offset = m_offset](double x, double y) -> double {
        return input(x, y, offset);
    });
}
