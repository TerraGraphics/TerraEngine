#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "graphics.h"


struct xcb_connection_t;
class VulkanGraphics : public Graphics {
public:
    VulkanGraphics() = delete;
    VulkanGraphics(uint32_t window, xcb_connection_t* connection);
    ~VulkanGraphics() = default;

    void Create(int validationLevel = -1) override;

    Diligent::EngineVkCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

private:
    uint32_t m_window;
    xcb_connection_t* m_connection;
    Diligent::EngineVkCreateInfo m_createInfo;
};
