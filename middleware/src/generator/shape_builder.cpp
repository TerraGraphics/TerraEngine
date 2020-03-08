#include "middleware/generator/shape_builder.h"

#include <cstdint>

#include "core/scene/vertexes.h"
#include "core/dg/render_device.h" // IWYU pragma: keep
#include "core/scene/index_buffer.h"
#include "core/scene/geometry_node.h"
#include "core/scene/vertex_buffer.h"
#include "middleware/generator/shape_generator.h"


ShapeBuilder::ShapeBuilder(const DevicePtr& device)
    : m_device(device) {

}

ShapeBuilder::~ShapeBuilder() {
    m_device.Release();
}

std::shared_ptr<GeometryNode> ShapeBuilder::Join(const std::initializer_list<const IShapeGenerator*>& shapes, const char* name) {
    VertexBufferBuilder vbBuilder;
    IndexBufferBuilder ibBuilder;

    uint32_t vertexStartIndex = 0;
    uint32_t indexCount = 0;
    for (auto& shape : shapes) {
        auto vb = vbBuilder.AddRange<VertexPNC>(shape->LenghtVertex());
        shape->FillVertex(vb.Begin());

        auto ib = ibBuilder.AddRange<uint32_t>(shape->LenghtIndex());
        shape->FillIndex(ib.Begin(), vertexStartIndex);

        vertexStartIndex += vb.Count();
        indexCount += ib.Count();
    }

    bool isUint32 = true;
    uint32_t vbOffsetBytes = 0;
    uint32_t ibOffsetBytes = 0;
    return std::make_shared<GeometryNodeIndexed>(vbBuilder.Build(m_device, name), vbOffsetBytes,
        ibBuilder.Build(m_device, name), ibOffsetBytes, indexCount, isUint32);
}
