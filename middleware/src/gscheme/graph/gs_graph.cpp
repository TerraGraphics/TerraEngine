#include "middleware/gscheme/graph/gs_graph.h"

namespace gs {

Graph::Graph(uint16_t initialNodeCount)
    : m_free(initialNodeCount)
    , m_capacity(initialNodeCount)
    , m_nodes(new Node[initialNodeCount]) {
}

Graph::~Graph() {
    if (m_nodes != nullptr) {
        delete[] m_nodes;
    }
}

}
