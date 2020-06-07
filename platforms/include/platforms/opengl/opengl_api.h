#pragma once

#include <cstdint>

#include "dg/graphics_types.h"
#include "platforms/graphic_api.h"


typedef struct _XDisplay Display;
class OpenGLAPI : public GraphicAPI {
public:
    OpenGLAPI() = delete;
    OpenGLAPI(uint32_t window, Display* display);
    ~OpenGLAPI() override = default;

    dg::EngineGLCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

    void Create(int validationLevel = -1) override;

private:
    dg::EngineGLCreateInfo m_createInfo;
};
