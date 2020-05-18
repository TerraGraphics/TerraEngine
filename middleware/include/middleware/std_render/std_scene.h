#pragma once

#include "core/scene/scene.h"


class WriteableVertexBuffer;
class StdScene : public Scene {
public:
    StdScene() = delete;
    StdScene(uint16_t vDeclIdPerInstance, bool addId);
    ~StdScene() = default;

public:
    std::shared_ptr<TransformNode> Update(uint32_t findId = 0);
    uint32_t Draw();

private:
    bool m_addId = false;
    uint32_t m_transformBufferBufferElementNumber = 0;
    std::shared_ptr<WriteableVertexBuffer> m_transformBuffer;
};
