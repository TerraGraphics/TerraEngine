#pragma once

#include <vector>
#include <cstdint>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"


namespace msh {
    class SemanticDecls;
}
namespace Diligent {
    struct LayoutElement;
}
namespace dg = Diligent;

struct VDeclItem;
class VDeclStorage : Fixed {
public:
    VDeclStorage();
    ~VDeclStorage();

    uint16_t Add(std::vector<VDeclItem>&& items);
    uint16_t Join(uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance);

    const msh::SemanticDecls& GetSemanticDecls(uint16_t id) const;
    const std::vector<dg::LayoutElement>& GetLayoutElements(uint16_t id) const;

private:
    struct Impl;
    Pimpl<Impl, 168, 8> impl;
};
