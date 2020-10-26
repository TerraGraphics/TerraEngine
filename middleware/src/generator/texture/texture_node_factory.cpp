#include "middleware/generator/texture/texture_node_factory.h"

#include <string>
#include <functional>

#include "dg/dg.h"
#include "core/common/exception.h"
#include "dg/default_raw_memory_allocator.h"
#include "middleware/graph_editor/graph_node.h"


TextureNodeFactory::TextureNodeFactory()
    : GraphNodeFactory (std::map<std::string, std::function<GraphNode* ()>>{}) {

}

TextureNodeFactory::~TextureNodeFactory() {
}

INodePreview* TextureNodeFactory::GetPreview(GraphNode* /* node */) {
    throw EngineError("TextureNodeFactory: unknown type for GetPreview");
}
