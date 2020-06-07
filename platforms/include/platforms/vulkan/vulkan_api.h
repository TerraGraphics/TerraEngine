#pragma once

#include <cstdint>

#include "dg/graphics_types.h"
#include "platforms/graphic_api.h"


struct xcb_connection_t;
class VulkanAPI : public GraphicAPI {
public:
    VulkanAPI() = delete;
    VulkanAPI(uint32_t window, xcb_connection_t* connection);
    ~VulkanAPI() override = default;

    dg::EngineVkCreateInfo& GetCreateInfo() noexcept { return m_createInfo; }

    void Create(int validationLevel = -1) override;

private:
    uint32_t m_window;
    xcb_connection_t* m_connection;
    dg::EngineVkCreateInfo m_createInfo;
};
