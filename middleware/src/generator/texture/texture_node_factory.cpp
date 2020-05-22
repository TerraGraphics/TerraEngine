#include "middleware/generator/texture/texture_node_factory.h"

#include <string>
#include <functional>

#include "core/dg/dg.h"
#include "core/common/exception.h"
#include "middleware/graph_editor/graph_node.h"
#include "core/dg/default_raw_memory_allocator.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/noise_pojection.h"
#include "middleware/generator/texture/noise_rasterization.h"


TextureNodeFactory::TextureNodeFactory()
    : GraphNodeFactory ({
        { CoherentNoise::GetName(), [this]() { return CreateCoherentNoise(); }},
        { PlaneProjection::GetName(), [this]() { return CreatePlaneProjection(); }},
        { NoiseToTexture::GetName(), [this]() { return CreateNoiseToTexture(); }},
    }) {

}

TextureNodeFactory::~TextureNodeFactory() {
}

CoherentNoise* TextureNodeFactory::CreateCoherentNoise() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), CoherentNoise::GetName(), CoherentNoise)();
}

PlaneProjection* TextureNodeFactory::CreatePlaneProjection() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), PlaneProjection::GetName(), PlaneProjection)();
}

NoiseToTexture* TextureNodeFactory::CreateNoiseToTexture() {
    return NEW_OBJ(dg::DefaultRawMemoryAllocator::GetAllocator(), NoiseToTexture::GetName(), NoiseToTexture)();
}

INodePreview* TextureNodeFactory::CompleteToPreview(CoherentNoise* node) {
    return CompleteToPreview(CreatePlaneProjection()->SetInputs(node));
}

INodePreview* TextureNodeFactory::CompleteToPreview(PlaneProjection* node) {
    return CompleteToPreview(CreateNoiseToTexture()->SetInputs(node));
}

INodePreview* TextureNodeFactory::CompleteToPreview(NoiseToTexture* node) {
    return node;
}

INodePreview* TextureNodeFactory::GetPreview(GraphNode* node) {
    if (auto* typedNode = dynamic_cast<CoherentNoise*>(node)) {
        return CompleteToPreview(typedNode);
    }

    if (auto* typedNode = dynamic_cast<PlaneProjection*>(node)) {
        return CompleteToPreview(typedNode);
    }

    if (auto* typedNode = dynamic_cast<NoiseToTexture*>(node)) {
        return CompleteToPreview(typedNode);
    }

    throw EngineError("TextureNodeFactory: unknown type for GetPreview");
}
