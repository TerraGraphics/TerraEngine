#pragma once

#include <cstdint>

#include "core/common/ctor.h"


namespace gs {

class IDraw : Fixed {

// Draw graph
public:
    virtual void OnStartDrawNode(uintptr_t id, const std::string& prettyName) = 0;
    virtual void OnFinishDrawNode() = 0;

    virtual void OnDrawPin(uintptr_t id, bool isInput, bool isConnected, const std::string& prettyName) = 0;
    virtual void OnDrawLink(uintptr_t linkId, uintptr_t srcPinId, uintptr_t dstPinId) = 0;

// Draw node edit GUI
public:
    virtual void OnDrawEditingHeader(const std::string& prettyName) = 0;
    // need return true if value chanded
    virtual bool OnDrawEditingPin(const std::string& prettyName, bool disabled, const rttr::variant& defaultValue, rttr::variant& value) = 0;
};

}