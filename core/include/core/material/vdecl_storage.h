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

class VDeclItem;
class VDeclStorage : Fixed {
public:
    VDeclStorage();
    ~VDeclStorage();

    uint32_t Add(std::vector<VDeclItem>&& items);
    uint32_t Join(uint32_t vDeclVertex, uint32_t vDeclinstance);

    const msh::SemanticDecls& GetSemanticDecls(uint32_t id) const;
    const std::vector<dg::LayoutElement>& GetLayoutElements(uint32_t id) const;

private:
    struct Impl;
    Pimpl<Impl, 168, 8> impl;
};
