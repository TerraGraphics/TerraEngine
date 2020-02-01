#include "core/scene/vertexes.h"


static dg::LayoutElement layoutElemsP[] = {
    // vertex position
    dg::LayoutElement{0, 0, 3, dg::VT_FLOAT32, dg::False},

    // Per-instance data - second buffer slot
    // WorldRow0
    dg::LayoutElement{1, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow1
    dg::LayoutElement{2, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow2
    dg::LayoutElement{3, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow3
    dg::LayoutElement{4, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow0
    dg::LayoutElement{5, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow1
    dg::LayoutElement{6, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow2
    dg::LayoutElement{7, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
};

static dg::LayoutElement layoutElemsPNC[] = {
    // vertex position
    dg::LayoutElement{0, 0, 3, dg::VT_FLOAT32, dg::False},
    // normal
    dg::LayoutElement{1, 0, 3, dg::VT_FLOAT32, dg::False},
    // texture coordinates
    dg::LayoutElement{2, 0, 2, dg::VT_FLOAT32, dg::False},

    // Per-instance data - second buffer slot
    // WorldRow0
    dg::LayoutElement{3, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow1
    dg::LayoutElement{4, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow2
    dg::LayoutElement{5, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // WorldRow3
    dg::LayoutElement{6, 1, 4, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow0
    dg::LayoutElement{7, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow1
    dg::LayoutElement{8, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
    // NormalRow2
    dg::LayoutElement{9, 1, 3, dg::VT_FLOAT32, dg::False, dg::LayoutElement::FREQUENCY_PER_INSTANCE},
};



const dg::InputLayoutDesc& VertexP::GetStandartLayoutDesc() {
    static const auto desc = dg::InputLayoutDesc(layoutElemsP, _countof(layoutElemsP));

    return desc;
}

const dg::InputLayoutDesc& VertexPNC::GetStandartLayoutDesc() {
    static const auto desc = dg::InputLayoutDesc(layoutElemsPNC, _countof(layoutElemsPNC));

    return desc;
}
