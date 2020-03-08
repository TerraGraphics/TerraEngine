#pragma once

#include <memory>
#include <initializer_list>

#include "core/dg/dg.h"


class GeometryNode;
class IShapeGenerator;
class ShapeBuilder {
public:
    ShapeBuilder() = delete;
    ShapeBuilder(const DevicePtr& device);
    ~ShapeBuilder();

    std::shared_ptr<GeometryNode> Join(const std::initializer_list<const IShapeGenerator*>& shapes, const char* name = nullptr);

private:
    DevicePtr m_device;
};
