#include "middleware/generator/texture/noise_pojection.h"


Noise2D::Noise2D(dg::IReferenceCounters* refCounters)
    : GraphNode(refCounters, 1) {

}

bool Noise2D::AttachInputImpl(uint8_t /* number */, GraphNode* node) {
    auto* noiseNode = dynamic_cast<Noise3D*>(node);
    if (noiseNode == nullptr) {
        return false;
    }

    m_noiseNode = noiseNode;
    return true;
}

PlaneProjection::PlaneProjection(dg::IReferenceCounters* refCounters)
    : Noise2D(refCounters) {

}

double PlaneProjection::Get(double u, double v) {
    return m_noiseNode->Get(u, v, m_coordZ);
}
