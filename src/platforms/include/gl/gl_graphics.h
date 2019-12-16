#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "graphics.h"


class GLGraphics : public Graphics {
public:
    GLGraphics() = delete;
    GLGraphics(void* display, void* nativeWindowHandle);
    ~GLGraphics() = default;

    void Create(int validationLevel = -1) override;

    Diligent::EngineGLCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

private:
    Diligent::EngineGLCreateInfo m_createInfo;
};
