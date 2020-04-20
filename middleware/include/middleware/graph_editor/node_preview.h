#pragma once

#include "core/dg/dg.h"
#include "core/common/ctor.h"


class INodePreview;
class NodePreview : Fixed {
public:
    NodePreview() = delete;
    NodePreview(bool isOpenGL);
    ~NodePreview();

    void SetNode(INodePreview* node);
    void ResetNode();
    void Draw();

private:
    bool m_isOpenGL = false;
    dg::RefCntWeakPtr<INodePreview> m_node;
};
