#include "middleware/generator/texture/section_plane.h"


math::Generator2d SectionPlaneX0Y::Result() const {
    return math::Generator2d([input = m_input, offset = m_offset](double x, double y) -> double {
        return input(x, y, offset);
    });
}
