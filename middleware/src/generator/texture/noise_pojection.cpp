#include "middleware/generator/texture/noise_pojection.h"


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

PlaneProjection::PlaneProjection(dg::IReferenceCounters* refCounters)
    : Noise2D(refCounters, GetName()) {

}

double PlaneProjection::Get(double u, double v) {
    return m_noiseNode->Get(u, v, m_coordZ);
}
