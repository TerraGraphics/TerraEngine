#pragma once

#include <vector>

#include "middleware/generator/empty_shape.h"
#include "middleware/generator/shape_generator.h"


template <class... T> class MergeShape;

template <> class MergeShape<> : public EmptyShape { };

template <typename Head, typename... Tail>
    class MergeShape<Head, Tail...> : public IShapeGenerator {
public:
    MergeShape(Head&& head, Tail&&... tail) :
		m_head{std::move(head)},
		m_tail{std::move(tail)...}
	{ }

    size_t LenghtVertex() const final {
        return m_head.LenghtVertex() + m_tail.LenghtVertex();
    }

    size_t LenghtIndex() const final {
        return m_head.LenghtIndex() + m_tail.LenghtIndex();
    }

    void FillVertex(VertexPNC* vertexes) const final {
        m_head.FillVertex(vertexes);
        m_tail.FillVertex(vertexes);
    }

    void FillIndex(uint32_t* indexes, uint32_t vertexStartIndex) const final {
        m_head.FillIndex(indexes, vertexStartIndex);
        m_tail.FillIndex(indexes, vertexStartIndex + m_head.LenghtVertex());
    }

private:
    Head m_head;
	MergeShape<Tail...> m_tail;
};
