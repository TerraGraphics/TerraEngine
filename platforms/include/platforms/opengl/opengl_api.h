#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>
#pragma clang diagnostic pop

#include "platforms/graphic_api.h"


typedef struct _XDisplay Display;
class OpenGLAPI : public GraphicAPI {
public:
    OpenGLAPI() = delete;
    OpenGLAPI(uint32_t window, Display* display);
    ~OpenGLAPI() override = default;

    Diligent::EngineGLCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

    void Create(int validationLevel = -1) override;

private:
    Diligent::EngineGLCreateInfo m_createInfo;
};
