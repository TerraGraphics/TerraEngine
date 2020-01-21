#pragma once

#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>

#include "platforms/graphic_api.h"


struct xcb_connection_t;
class VulkanAPI : public GraphicAPI {
public:
    VulkanAPI() = delete;
    VulkanAPI(uint32_t window, xcb_connection_t* connection);
    ~VulkanAPI() override = default;

    Diligent::EngineVkCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

    void Create(int validationLevel = -1) override;

private:
    uint32_t m_window;
    xcb_connection_t* m_connection;
    Diligent::EngineVkCreateInfo m_createInfo;
};
