#include "middleware/generator/texture/noise_pojection.h"

#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"
#include "middleware/generator/texture/coherent_noise.h"
#include "middleware/generator/texture/texture_consts.h"


Noise2D::Noise2D(dg::IReferenceCounters* refCounters, const char* name)
    : GraphNode(refCounters, name, Noise2D::OutputTypeID(), {Noise3D::OutputTypeID()}) {

}

bool Noise2D::AttachInputImpl(uint8_t /* number */, GraphNode* node) {
    auto* noiseNode = dynamic_cast<Noise3D*>(node);
    if (noiseNode == nullptr) {
        return false;
    }

    m_noiseNode = noiseNode;
    return true;
}

bool Noise2D::DetachInputImpl(uint8_t /* number */) {
    m_noiseNode = nullptr;
    return true;
}

uint32_t Noise2D::OutputTypeID() {
    return static_cast<uint32_t>(TexturePinType::Noise2D);
}
