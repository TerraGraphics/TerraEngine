#pragma once

#include <vector>
#include <string>
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

    bool IsNameExists(uint16_t vDeclId, const std::string& name) const;
    const msh::SemanticDecls& GetSemanticDecls(uint16_t id) const;
    const std::vector<dg::LayoutElement>& GetLayoutElements(uint16_t id) const;

private:
    struct Impl;
    Pimpl<Impl, 224, 8> impl;
};
