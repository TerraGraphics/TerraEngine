#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "platforms/graphic_api.h"


typedef struct _XDisplay Display;
class OpenGLAPI : public GraphicAPI {
public:
    OpenGLAPI() = delete;
    OpenGLAPI(void* nativeWindowHandler, Display* display);
    ~OpenGLAPI() override = default;

    Diligent::EngineGLCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

    void Create(int validationLevel = -1) override;

private:
    Diligent::EngineGLCreateInfo m_createInfo;
};
