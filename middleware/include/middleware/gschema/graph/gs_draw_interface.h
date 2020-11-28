#pragma once

#include <vector>
#include <cstdint>
#include <string_view>

#include "core/common/ctor.h"
#include "middleware/gschema/graph/gs_types_decl.h"


namespace cpgf {
    class GVariant;
}
namespace gs {

class TypeInstance;
class IDraw : Fixed {

// Draw graph
public:
    struct Pin {
        uintptr_t id;
        bool isConnected;
        std::string displayName;
    };

public:
    virtual void OnStartDrawGraph() = 0;
    virtual void OnFinishDrawGraph() = 0;

    virtual void OnStartDrawNode(uintptr_t id, std::string displayName) = 0;
    virtual void OnFinishDrawNode(bool isValid, std::string_view errorMessage) = 0;

    virtual void OnDrawInputPins(const std::vector<Pin>& pins) = 0;
    virtual void OnDrawMiniPreview(TypeId valueTypeId, const cpgf::GVariant& value, uint8_t valueVersion) = 0;
    virtual void OnDrawOutputPins(const std::vector<Pin>& pins) = 0;
    virtual void OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) = 0;

    // nodeId == 0 for disable preview
    virtual void OnDrawFullPreview(uint16_t nodeId, TypeId valueTypeId, const cpgf::GVariant& value, uint8_t valueVersion) = 0;

// Draw node edit GUI
public:
    enum class ButtonsState : uint8_t {
        None = 0,
        ResetToDefault = 1,
    };
    virtual void OnStartDrawNodeProperty(const std::string& displayName) = 0;
    virtual ButtonsState OnDrawPinProperty(const std::string& displayName, TypeInstance* typeInstance, bool disabled) = 0;
    virtual void OnFinishDrawNodeProperty() = 0;
};

}
