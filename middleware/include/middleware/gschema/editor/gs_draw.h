#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <string_view>

#include "dg/dg.h"
#include "core/math/types.h"
#include "middleware/gschema/editor/gs_draw_node.h"
#include "middleware/gschema/graph/gs_types_decl.h"
#include "middleware/gschema/graph/gs_draw_interface.h"


namespace cpgf {
    class GVariant;
}

namespace gs {

class DrawPreview;
class TypeInstance;
class Draw final : public IDraw {
public:
    Draw() = delete;
    Draw(TexturePtr& texBackground);
    ~Draw();

// Draw graph
public:
    void OnStartDrawGraph() final;
    void OnFinishDrawGraph() final;

    void OnStartDrawNode(uintptr_t id, std::string displayName) final;
    void OnFinishDrawNode(bool isValid, std::string_view errorMessage) final;

    void OnDrawInputPins(const std::vector<IDraw::Pin>& pins) final;
    void OnDrawMiniPreview(TypeId valueTypeId, const cpgf::GVariant& value, uint8_t valueVersion) final;
    void OnDrawOutputPins(const std::vector<IDraw::Pin>& pins) final;
    void OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) final;

    // nodeId == 0 for disable preview
    void OnDrawFullPreview(uint16_t nodeId, TypeId valueTypeId, const cpgf::GVariant& value, uint8_t valueVersion) final;

// Draw node edit GUI
public:
    void OnStartDrawNodeProperty(const std::string& displayName) final;
    ButtonsState OnDrawPinProperty(const std::string& displayName, TypeInstance* typeInstance, bool disabled) final;
    void OnFinishDrawNodeProperty() final;

// tmp data for draw frame
private:
    uint8_t m_alpha = 0;
    uint16_t m_previewNodeId = 0;
    DrawNode* m_node = nullptr;

// persistent data
private:
    // key = nodeIndex
    std::vector<DrawNode> m_nodes;
    math::SizeF m_texBackgroundSize;
    TextureViewPtr m_texBackground;
    DrawPreview* m_preview = nullptr;
};

}
