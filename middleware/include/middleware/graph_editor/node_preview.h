#pragma once

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class NoiseRasterization2D;
class NodePreview : Fixed {
public:
    NodePreview() = delete;
    NodePreview(bool isOpenGL);
    ~NodePreview() = default;

    void SetNode(NoiseRasterization2D* node);
    void ResetNode();
    void Draw();

private:
    bool m_isOpenGL = false;
    dg::RefCntAutoPtr<NoiseRasterization2D> m_node;
};
