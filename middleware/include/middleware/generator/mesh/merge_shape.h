#pragma once

#include <vector>

#include "core/common/ctor.h"
#include "middleware/generator/mesh/empty_shape.h"
#include "middleware/generator/mesh/shape_generator.h"


template <class... T> class MergeShape;

template <> class MergeShape<> : public EmptyShape, Noncopyable {
public:
    MergeShape() = default;
    MergeShape(MergeShape&&) noexcept {

    }

    MergeShape& operator=(MergeShape&&) noexcept {
        return *this;
    }
};

template <typename Head, typename... Tail>
    class MergeShape<Head, Tail...> : public IShapeGenerator, Noncopyable {
public:
    MergeShape(Head&& head, Tail&&... tail)
		: m_head(std::move(head))
		, m_tail(std::move(tail)...) {
    }

    MergeShape(MergeShape<Head, Tail...>&& other) noexcept
        : m_head(std::move(other.m_head))
        , m_tail(std::move(other.m_tail)) {

    }

    MergeShape& operator=(MergeShape<Head, Tail...>&& other) noexcept {
        m_head = std::move(other.m_head);
        m_tail = std::move(other.m_tail);

        return *this;
    }

    size_t LenghtVertex() const final {
        auto res = m_head.LenghtVertex() + m_tail.LenghtVertex();
        return res;
    }

    size_t LenghtIndex() const final {
        return m_head.LenghtIndex() + m_tail.LenghtIndex();
    }

    void FillVertex(VertexPNC* vertexes) const final {
        auto* vb = vertexes;
        m_head.FillVertex(vb);
        vb += m_head.LenghtVertex();
        m_tail.FillVertex(vb);
    }

    void FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const final {
        auto* ib = indexes;
        m_head.FillIndex(ib, vertexStartIndex);
        ib += m_head.LenghtIndex();
        m_tail.FillIndex(ib, vertexStartIndex + static_cast<uint32_t>(m_head.LenghtVertex()));
    }

private:
    Head m_head;
	MergeShape<Tail...> m_tail;
};
