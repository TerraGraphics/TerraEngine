#include "middleware/generator/texture/noise_pojection.h"

#include "core/common/exception.h"
#include "middleware/imgui/widgets.h"


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

double PlaneProjection::GetCoordZ() const {
    return m_coordZ;
}

void PlaneProjection::SetCoordZ(double value) {
    m_coordZ = value;
    StateChanged();
}

PlaneProjection* PlaneProjection::SetInputs(Noise3D* input) {
    if (!AttachInput(0, input)) {
        throw EngineError("PlaneProjection: can't set input number 0");
    }

    return this;
}

double PlaneProjection::Get(double u, double v) {
    if (m_noiseNode == nullptr) {
        throw EngineError("PlaneProjection: one of the inputs is empty");
    }

    return m_noiseNode->Get(u, v, m_coordZ);
}

void PlaneProjection::DrawGui() {
    if (gui::InputScalar("Z coordinate", m_coordZ, 0.1, "%.1f")) {
        StateChanged();
    }
}
