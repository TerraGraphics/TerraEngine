#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "platforms/graphics.h"


typedef struct _XDisplay Display;
class GLGraphics : public Graphics {
public:
    GLGraphics() = delete;
    GLGraphics(uint32_t window, Display* display);
    ~GLGraphics() override = default;

    Diligent::EngineGLCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

    void Create(int validationLevel = -1) override;

private:
    Diligent::EngineGLCreateInfo m_createInfo;
};
