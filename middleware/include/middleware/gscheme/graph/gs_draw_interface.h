#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gscheme/graph/gs_types_decl.h"


namespace cpgf {
    class GVariant;
}
namespace gs {

class IDraw : Fixed {

// Draw graph
public:
    struct Pin {
        uintptr_t id;
        bool isConnected;
        std::string prettyName;
    };

public:
    virtual void OnStartDrawGraph() = 0;
    virtual void OnFinishDrawGraph() = 0;

    virtual void OnStartDrawNode(uintptr_t id, std::string prettyName) = 0;
    virtual void OnFinishDrawNode(bool isValid) = 0;

    virtual void OnDrawInputPins(const std::vector<Pin>& pins) = 0;
    virtual void OnDrawPinPreview(TypeId typeId, const cpgf::GVariant& value) = 0;
    virtual void OnDrawOutputPins(const std::vector<Pin>& pins) = 0;
    virtual void OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) = 0;

// Draw node edit GUI
public:
    enum class EditResult : uint8_t {
        NotChanged = 0,
        Changed = 1,
        ResetToDefault = 2,
    };
    virtual void OnDrawEditingHeader(const std::string& prettyName) = 0;
    // need return true if value chanded
    virtual EditResult OnDrawEditingPin(const std::string& prettyName, bool disabled, TypeId typeId, cpgf::GVariant& value) = 0;
};

}
