#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "dg/dg.h"
#include "core/math/types.h"
#include "middleware/gscheme/editor/gs_node.h"
#include "middleware/gscheme/graph/gs_types_decl.h"
#include "middleware/gscheme/graph/gs_draw_interface.h"


namespace cpgf {
    class GVariant;
}

namespace gs {

class Draw final : public IDraw {
public:
    Draw() = delete;
    Draw(TexturePtr& texBackground);
    ~Draw();

// Draw graph
public:
    void OnStartDrawGraph() final;
    void OnFinishDrawGraph() final;
    void OnStartDrawNode(uintptr_t id, std::string prettyName) final;
    void OnFinishDrawNode() final;
    void OnDrawInputPins(const std::vector<IDraw::Pin>& pins) final;
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) final;
    void OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) final;

// Draw node edit GUI
public:
    void OnDrawEditingHeader(const std::string& prettyName) final;
    EditResult OnDrawEditingPin(const std::string& prettyName, bool disabled, TypeId typeId, cpgf::GVariant& value) final;

// tmp data for draw frame
private:
    uint8_t m_alpha = 0;
    Node* m_node = nullptr;

// cache data
private:
    // key = nodeIndex
    std::vector<Node> m_nodes;

// persistent data
private:
    const float m_nodePaddingLeft = 8;
    const float m_nodePaddingRight = 8;
    const float m_nodePaddingTop = 4;
    const float m_nodePaddingBottom = 8;

    math::SizeF m_texBackgroundSize;
    TextureViewPtr m_texBackground;
};

}
