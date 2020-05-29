#pragma once

#include <functional>

#include "core/common/ctor.h"
#include "core/render/vertexes.h"
#include "middleware/generator/mesh/shape_generator.h"


template<class TShape>
    class VertexEvalApplyer : public IShapeGenerator, Noncopyable {
public:
    using Callback = std::function<void (VertexPNC* /* begin */, VertexPNC* /* end */)>;
    VertexEvalApplyer() = delete;
    VertexEvalApplyer(TShape baseGenerator, Callback&& callback)
        : m_baseGenerator(std::move(baseGenerator))
        , m_callback(std::move(callback)) {

    }

    VertexEvalApplyer(VertexEvalApplyer&& other) noexcept
        : m_baseGenerator(std::move(other.m_baseGenerator))
        , m_callback(std::move(other.m_callback)) {

    }

    VertexEvalApplyer& operator=(VertexEvalApplyer&& other) noexcept {
        m_baseGenerator = std::move(other.m_baseGenerator);
        m_callback = std::move(other.m_callback);

        return *this;
    }

    size_t LenghtVertex() const final {
        return m_baseGenerator.LenghtVertex();
    }

    size_t LenghtIndex() const final {
        return m_baseGenerator.LenghtIndex();
    }

    void FillVertex(VertexPNC* vertexes) const final {
        m_baseGenerator.FillVertex(vertexes);
        m_callback(vertexes, vertexes + m_baseGenerator.LenghtVertex());
    }

    void FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const final {
        m_baseGenerator.FillIndex(indexes, vertexStartIndex);
    }

private:
    TShape m_baseGenerator;
    Callback m_callback;
};
