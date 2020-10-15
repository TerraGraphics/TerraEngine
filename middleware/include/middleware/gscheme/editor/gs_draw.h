#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "dg/dg.h"
#include "core/math/types.h"
#include "middleware/gscheme/editor/gs_draw_node.h"
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
    DrawNode* m_node = nullptr;

// persistent data
private:
    // key = nodeIndex
    std::vector<DrawNode> m_nodes;
    math::SizeF m_texBackgroundSize;
    TextureViewPtr m_texBackground;
};

}
