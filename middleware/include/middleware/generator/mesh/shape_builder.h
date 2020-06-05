#pragma once

#include <memory>
#include <initializer_list>

#include "dg/dg.h"


class Geometry;
class IShapeGenerator;
class ShapeBuilder {
public:
    ShapeBuilder() = delete;
    ShapeBuilder(const DevicePtr& device);
    ~ShapeBuilder();

    std::shared_ptr<Geometry> Join(const std::initializer_list<const IShapeGenerator*>& shapes, const char* name = nullptr);

private:
    DevicePtr m_device;
};
