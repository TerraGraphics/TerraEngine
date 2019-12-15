#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "graphics.h"


class GLGraphics : public Graphics {
public:
    GLGraphics() = delete;
    GLGraphics(void* display, void* nativeWindowHandle);
    ~GLGraphics() = default;

    void Create();
    void Destroy();

    Diligent::EngineGLCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

private:
    Diligent::EngineGLCreateInfo m_createInfo;
};
