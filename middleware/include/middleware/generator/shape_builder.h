#pragma once

#include <memory>
#include <initializer_list>

#include "core/dg/dg.h"


class Shape;
class GeometryNode;
class ShapeBuilder {
public:
    ShapeBuilder() = delete;
    ShapeBuilder(const DevicePtr& device);
    ~ShapeBuilder();

    std::shared_ptr<GeometryNode> Join(const std::initializer_list<const Shape*>& shapes, const dg::Char* name = nullptr);

private:
    DevicePtr m_device;
};
