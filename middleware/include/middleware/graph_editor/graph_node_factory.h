#pragma once

#include <map>
#include <functional>
#include "core/common/ctor.h"


class GraphNode;
class GraphNodeFactory : Fixed {
protected:
    GraphNodeFactory() = delete;
    GraphNodeFactory(std::map<std::string, std::function<GraphNode* ()>>&& nodeCtors) : m_nodeCtors(std::move(nodeCtors)) {}

public:
    const std::map<std::string, std::function<GraphNode* ()>> GetNodes() { return m_nodeCtors; }

private:
    const std::map<std::string, std::function<GraphNode* ()>> m_nodeCtors;
};