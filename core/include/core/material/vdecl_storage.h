#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "core/common/ctor.h"
#include "core/common/pimpl.h"
#include "core/common/named_type.h"


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
    using VarNameId = NamedType<uint16_t, struct VarNameIdTag>;

    VDeclStorage();
    ~VDeclStorage();

    // valid id more than 0
    uint16_t Add(std::vector<VDeclItem>&& items);
    // valid id more than 0
    uint16_t Join(uint16_t vDeclIdPerVertex, uint16_t vDeclIdPerInstance);

    VarNameId GetVarNameId(const std::string& name);
    bool IsNameExists(VarNameId varNameId, uint16_t vDeclId) const;

    const msh::SemanticDecls& GetSemanticDecls(uint16_t id) const;
    const std::vector<dg::LayoutElement>& GetLayoutElements(uint16_t id) const;

private:
    struct Impl;
    Pimpl<Impl, 304, 8> impl;
};
